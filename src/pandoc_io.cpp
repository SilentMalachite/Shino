#include "pandoc_io.h"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>

namespace ShinoEditor {

bool PandocIO::IsPandocAvailable() {
    int result = std::system("pandoc --version > /dev/null 2>&1");
    return result == 0;
}

std::optional<std::string> PandocIO::ImportDocx(const std::string& docx_path) {
    if (!IsPandocAvailable()) {
        return std::nullopt;
    }
    
    std::string command = "pandoc -f docx -t markdown " + ShellEscape(docx_path);
    std::string result = ExecutePandocCommand(command);
    
    if (result.empty()) {
        return std::nullopt;
    }
    
    return result;
}

bool PandocIO::ExportDocx(const std::string& markdown_content, const std::string& docx_path) {
    if (!IsPandocAvailable()) {
        return false;
    }
    
    // Write markdown to temporary file
    std::string temp_file = "/tmp/sino_temp.md";
    std::ofstream temp_out(temp_file);
    if (!temp_out) {
        return false;
    }
    temp_out << markdown_content;
    temp_out.close();
    
    // Convert to DOCX
    std::string command = "pandoc -f markdown -t docx " + ShellEscape(temp_file) + " -o " + ShellEscape(docx_path);
    std::string result = ExecutePandocCommand(command);
    
    // Clean up temp file
    std::remove(temp_file.c_str());
    
    // Check if output file was created
    std::ifstream check(docx_path);
    return check.good();
}

std::string PandocIO::GetPandocVersion() {
    if (!IsPandocAvailable()) {
        return "Pandoc not available";
    }
    
    return ExecutePandocCommand("pandoc --version | head -1");
}

std::string PandocIO::ExecutePandocCommand(const std::string& command) {
    std::string result;
    
#ifdef _WIN32
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(command.c_str(), "r"), _pclose);
#else
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
#endif
    
    if (!pipe) {
        return "";
    }
    
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
        result += buffer;
    }
    
    return result;
}

std::string PandocIO::ShellEscape(const std::string& arg) {
#ifdef _WIN32
    // 簡易対応: ダブルクォートをエスケープしつつ全体をダブルクォートで囲む
    std::string out;
    out.reserve(arg.size() + 2);
    out.push_back('"');
    for (char c : arg) {
        if (c == '"') out += '"';
        out += c;
    }
    out.push_back('"');
    return out;
#else
    // POSIX: シングルクォートで囲み、内部の ' は '\'' に分割して表現
    std::string out;
    out.reserve(arg.size() + 2);
    out.push_back(static_cast<char>(39));
    for (char c : arg) {
        if (c == static_cast<char>(39)) {
            out += "'\\''"; // 終了→エスケープ→再開始
        } else {
            out.push_back(c);
        }
    }
    out.push_back(static_cast<char>(39));
    return out;
#endif
}

}
