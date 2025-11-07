#pragma once
#include "error_handler.h"
#include <string>
#include <filesystem>
#include <regex>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <chrono>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

namespace fs = std::filesystem;

namespace ShinoEditor {
namespace security {

// Security error category
class SecurityError : public ShinoError {
public:
    SecurityError(const std::string& msg, const std::string& detail = "")
        : ShinoError(Category::System, msg, detail) {}
};

// File path sanitization and validation
class PathValidator {
public:
    static void ValidatePathSecurity(const std::string& path) {
        if (path.empty()) {
            throw SecurityError("Empty path", "Provided path is empty");
        }
        // Reject NUL byte and non-printable control chars to prevent injection
        for (unsigned char c : path) {
            if (c == '\0' || (c < 0x20 && c != '\t' && c != '\n')) {
                throw SecurityError("Invalid control character in path", "Path contains non-printable characters");
            }
        }
        // Normalize and validate path first, then apply policy checks if needed
        try {
            fs::path norm_path = fs::weakly_canonical(fs::absolute(path));
            (void)norm_path; // reserved for future policy checks
        } catch (const fs::filesystem_error& e) {
            throw SecurityError(
                "Path validation failed",
                e.what()
            );
        }
    }
    
    static void ValidateFileOperation(const std::string& path, bool write = false) {
        // First validate path security
        ValidatePathSecurity(path);
        
        fs::path fs_path = fs::absolute(path);
        
        // Check if path exists
        bool exists = fs::exists(fs_path);
        
        if (write) {
            // For write operations
            if (exists) {
                // Check if file (not directory)
                if (!fs::is_regular_file(fs_path)) {
                    throw SecurityError(
                        "Invalid file type",
                        "Path exists but is not a regular file: " + path
                    );
                }
                
                // Check write permission
                std::ofstream test(fs_path, std::ios::app);
                if (!test) {
                    throw SecurityError(
                        "File not writable",
                        "Cannot write to file: " + path
                    );
                }
            } else {
                // Check if parent directory exists and is writable
                fs::path parent = fs_path.parent_path();
                if (!fs::exists(parent)) {
                    throw SecurityError(
                        "Parent directory not found",
                        "Directory does not exist: " + parent.string()
                    );
                }
                if (!fs::is_directory(parent)) {
                    throw SecurityError(
                        "Invalid parent path",
                        "Parent path is not a directory: " + parent.string()
                    );
                }
                
                // Check directory write permission by attempting to create a test file
                try {
#ifdef _WIN32
                    // Windows: Use CreateFile API
                    std::string test_file = (parent / ("test_write_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".tmp")).string();
                    std::ofstream test(test_file);
                    if (!test) {
                        throw SecurityError(
                            "Directory not writable",
                            "Cannot create files in directory: " + parent.string()
                        );
                    }
                    test.close();
                    fs::remove(test_file);
#else
                    std::string test_file_template = (parent / "test_write_XXXXXX").string();
                    // Create a mutable buffer for mkstemp
                    std::vector<char> test_file_buf(test_file_template.begin(), test_file_template.end());
                    test_file_buf.push_back('\0');
                    
                    int fd = mkstemp(test_file_buf.data());
                    if (fd == -1) {
                        throw SecurityError(
                            "Directory not writable",
                            "Cannot create files in directory: " + parent.string()
                        );
                    }
                    close(fd);
                    std::string test_file(test_file_buf.begin(), test_file_buf.end() - 1);
                    fs::remove(test_file);
#endif
                } catch (const SecurityError&) {
                    throw; // Re-throw SecurityError as-is
                } catch (const std::exception& e) {
                    throw SecurityError(
                        "Directory write check failed",
                        e.what()
                    );
                }
            }
        } else {
            // For read operations
            if (!exists) {
                throw SecurityError(
                    "File not found",
                    "Path does not exist: " + path
                );
            }
            
            if (!fs::is_regular_file(fs_path)) {
                throw SecurityError(
                    "Invalid file type",
                    "Path exists but is not a regular file: " + path
                );
            }
            
            // Check read permission
            std::ifstream test(fs_path);
            if (!test) {
                throw SecurityError(
                    "File not readable",
                    "Cannot read file: " + path
                );
            }
        }
    }
};

// Command execution security
class CommandValidator {
public:
    static void ValidateCommand(const std::string& cmd) {
        // Check for shell metacharacters and other dangerous patterns
        const std::vector<std::regex> dangerous_patterns = {
            std::regex("[&|;`$]"),  // Shell metacharacters
            std::regex(">"),        // Redirection
            std::regex("<"),        // Redirection
            std::regex("\\\\"),     // Escapes
            std::regex("\\*"),      // Wildcards
            std::regex("\\?"),      // Wildcards
            std::regex("\\["),      // Character classes
            std::regex("\\]"),      // Character classes
            std::regex("~"),        // Home directory
            std::regex("\\{"),      // Brace expansion
            std::regex("\\}"),      // Brace expansion
            std::regex("\\("),      // Subshells
            std::regex("\\)"),      // Subshells
        };
        
        for (const auto& pattern : dangerous_patterns) {
            if (std::regex_search(cmd, pattern)) {
                throw SecurityError(
                    "Invalid command",
                    "Command contains dangerous characters: " + cmd
                );
            }
        }
        
        // Additional command validation can be added here
        // For example, whitelisting specific commands
    }
    
    static std::string SafeShellEscape(const std::string& arg) {
#ifdef _WIN32
        // For Windows: escape and quote
        std::string escaped;
        escaped.reserve(arg.size() * 2 + 2);
        escaped.push_back('"');
        for (char c : arg) {
            if (c == '"' || c == '\\') {
                escaped.push_back('\\');
            }
            escaped.push_back(c);
        }
        escaped.push_back('"');
        return escaped;
#else
        // For POSIX: use single quotes
        std::string escaped;
        escaped.reserve(arg.size() * 4 + 2);
        escaped.push_back('\'');
        for (char c : arg) {
            if (c == '\'') {
                escaped += "'\\''"; // Close quote, escaped quote, reopen quote
            } else {
                escaped.push_back(c);
            }
        }
        escaped.push_back('\'');
        return escaped;
#endif
    }
    
    static std::string BuildSafeCommand(const std::string& program, 
                                      const std::vector<std::string>& args) {
        std::string cmd = SafeShellEscape(program);
        for (const auto& arg : args) {
            cmd += " " + SafeShellEscape(arg);
        }
        return cmd;
    }
};

} // namespace security
} // namespace ShinoEditor