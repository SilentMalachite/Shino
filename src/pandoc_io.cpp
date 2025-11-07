#include "pandoc_io.h"
#include "security.h"
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <random>
#include <chrono>
#include <filesystem>
#include <system_error>

namespace fs = std::filesystem;

namespace ShinoEditor {

// Generate a secure temporary file name
std::string PandocIO::GenerateTempFileName() {
    fs::path dir;
    try {
        dir = fs::temp_directory_path();
    } catch (...) {
#ifdef _WIN32
        dir = "C:/Windows/Temp";
#else
        dir = "/tmp";
#endif
    }
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dist;

    for (int attempt = 0; attempt < 64; ++attempt) {
        const auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        std::ostringstream oss;
        oss << "shino_temp_" << std::hex << now << '_' << dist(gen) << ".md";
        const fs::path candidate = dir / oss.str();

        std::error_code ec;
        if (!fs::exists(candidate, ec)) {
            return candidate.string();
        }
    }

    // Fallback in the unlikely event all attempts fail
    return (dir / "shino_temp_fallback.md").string();
}

bool PandocIO::IsPandocAvailable() {
    try {
        // Execute pandoc --version and consider it available if we get any output
        std::string cmd = security::CommandValidator::BuildSafeCommand("pandoc", {"--version"});
        std::string output = ExecutePandocCommand(cmd);
        return !output.empty();
    } catch (...) {
        return false;
    }
}

std::optional<std::string> PandocIO::ImportDocx(const std::string& docx_path) {
    // Verify pandoc availability
    if (!IsPandocAvailable()) {
        return std::nullopt;
    }
    
    // Validate file security and permissions
    try {
        security::PathValidator::ValidateFileOperation(docx_path, false);
    } catch (const security::SecurityError&) {
        return std::nullopt;
    }
    
    // Verify file has .docx extension
    if (fs::path(docx_path).extension() != ".docx") {
        return std::nullopt;
    }
    
    // Build and validate command
    std::vector<std::string> args = {
        "-f", "docx",
        "-t", "markdown",
        docx_path
    };
    
    try {
        // Build and execute pandoc command
        std::string command = security::CommandValidator::BuildSafeCommand("pandoc", args);
        std::string result = ExecutePandocCommand(command);
        
        if (result.empty()) {
            return std::nullopt;
        }
        
        return result;
    } catch (...) {
        return std::nullopt;
    }
}

bool PandocIO::ExportDocx(const std::string& markdown_content, const std::string& docx_path) {
    // Verify pandoc availability
    if (!IsPandocAvailable()) {
        return false;
    }
    
    // Verify output file has .docx extension
    if (fs::path(docx_path).extension() != ".docx") {
        return false;
    }
    
    // Validate output file security and permissions
    try {
        security::PathValidator::ValidateFileOperation(docx_path, true);
    } catch (const security::SecurityError&) {
        return false;
    }
    
    // Create and validate temporary file
    std::string temp_file = GenerateTempFileName();
    
    try {
        security::PathValidator::ValidateFileOperation(temp_file, true);
    } catch (const security::SecurityError&) {
        return false;
    }
    
    // Write content to temp file
    std::ofstream temp_out(temp_file);
    if (!temp_out) {
        return false;
    }
    temp_out << markdown_content;
    temp_out.close();
    
    // Ensure temp file was written
    if (!fs::exists(temp_file)) {
        return false;
    }
    
    // Build and validate command
    std::vector<std::string> args = {
        "-f", "markdown",
        "-t", "docx",
        temp_file,
        "-o", docx_path
    };
    
    try {
        std::string command = security::CommandValidator::BuildSafeCommand("pandoc", args);
        std::string result = ExecutePandocCommand(command);
        
        // Clean up temp file
        try {
            fs::remove(temp_file);
        } catch (...) {
            // Log but don't throw - temp file cleanup failure isn't critical
            std::cerr << "Warning: Failed to remove temporary file: " << temp_file << std::endl;
        }
        
        // Check if output file was created and is readable
        if (!fs::exists(docx_path)) {
            return false;
        }
        
        return true;
    } catch (...) {
        // Clean up temp file
        try {
            fs::remove(temp_file);
        } catch (...) {
            // Ignore cleanup errors
        }
        return false;
    }
}

std::string PandocIO::GetPandocVersion() {
    if (!IsPandocAvailable()) {
        error::ThrowSystemError("version check", "pandoc is not available");
    }
    std::string command = security::CommandValidator::BuildSafeCommand("pandoc", {"--version"});
    std::string output = ExecutePandocCommand(command);
    if (output.empty()) {
        error::ThrowSystemError("version check", "failed to get pandoc version");
    }
    std::istringstream stream(output);
    std::string first_line;
    std::getline(stream, first_line);
    if (first_line.empty()) {
        error::ThrowSystemError("version check", "failed to parse pandoc version");
    }
    return first_line;
}

std::string PandocIO::ExecutePandocCommand(const std::string& command) {
#ifdef _WIN32
    FILE* pipe = _popen(command.c_str(), "r");
#else
    FILE* pipe = popen(command.c_str(), "r");
#endif
    if (!pipe) {
        return "";
    }

    std::string result;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

#ifdef _WIN32
    int status = _pclose(pipe);
#else
    int status = pclose(pipe);
#endif
    if (status != 0) {
        return "";
    }

    return result;
}

std::string PandocIO::ShellEscape(const std::string& arg) {
    // Use the security framework's shell escaping
    return security::CommandValidator::SafeShellEscape(arg);
}

}
