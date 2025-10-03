#pragma once
#include <string>
#include <exception>
#include <sstream>

namespace ShinoEditor {

// Custom exception class with error category and details
class ShinoError : public std::exception {
public:
    enum class Category {
        File,      // File operations
        Parser,    // Markdown parsing
        Convert,   // Format conversion
        UI,        // User interface
        System     // System/OS errors
    };

    ShinoError(Category cat, const std::string& msg, const std::string& detail = "") 
        : category_(cat), message_(msg), detail_(detail) {
        std::ostringstream oss;
        oss << CategoryToString(category_) << ": " << message_;
        if (!detail_.empty()) {
            oss << " (" << detail_ << ")";
        }
        full_message_ = oss.str();
    }

    const char* what() const noexcept override {
        return full_message_.c_str();
    }

    Category category() const { return category_; }
    const std::string& message() const { return message_; }
    const std::string& detail() const { return detail_; }

private:
    static const char* CategoryToString(Category cat) {
        switch (cat) {
            case Category::File: return "File Error";
            case Category::Parser: return "Parser Error";
            case Category::Convert: return "Conversion Error";
            case Category::UI: return "UI Error";
            case Category::System: return "System Error";
            default: return "Unknown Error";
        }
    }

    Category category_;
    std::string message_;
    std::string detail_;
    std::string full_message_;
};

// Helper functions for common error cases
namespace error {
    inline void ThrowFileNotFound(const std::string& path) {
        throw ShinoError(
            ShinoError::Category::File,
            "File not found",
            path
        );
    }

    inline void ThrowFileNotWritable(const std::string& path) {
        throw ShinoError(
            ShinoError::Category::File,
            "Cannot write to file",
            path
        );
    }

    inline void ThrowConversionFailed(const std::string& from, const std::string& to, const std::string& detail) {
        throw ShinoError(
            ShinoError::Category::Convert,
            "Failed to convert " + from + " to " + to,
            detail
        );
    }

    inline void ThrowSystemError(const std::string& operation, const std::string& detail) {
        throw ShinoError(
            ShinoError::Category::System,
            "System error during " + operation,
            detail
        );
    }
}

} // namespace ShinoEditor