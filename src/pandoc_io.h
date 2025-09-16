#pragma once
#include <string>
#include <optional>

namespace ShinoEditor {

class PandocIO {
public:
    // Check if pandoc is available
    static bool IsPandocAvailable();
    
    // Import DOCX file to markdown
    static std::optional<std::string> ImportDocx(const std::string& docx_path);
    
    // Export markdown to DOCX file
    static bool ExportDocx(const std::string& markdown_content, const std::string& docx_path);
    
    // Get pandoc version
    static std::string GetPandocVersion();
    
private:
    // Execute pandoc command
    static std::string ExecutePandocCommand(const std::string& command);

    // Shell安全のための簡易エスケープ（主にPOSIX想定）
    static std::string ShellEscape(const std::string& arg);
};

}
