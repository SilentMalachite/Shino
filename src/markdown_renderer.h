#pragma once
#include <string>
#include <vector>

namespace ShinoEditor {

class MarkdownRenderer {
public:
    MarkdownRenderer();
    ~MarkdownRenderer();
    
    // Render markdown text to HTML
    std::string RenderToHtml(const std::string& markdown) const;
    
    // Render markdown to plain text (for preview)
    std::string RenderToText(const std::string& markdown) const;
    
    // Check if md4c is available
    static bool IsAvailable();
    
private:
    // MD4C callback functions
    static int ProcessBlock(int block_type, void* detail, void* userdata);
    static int ProcessSpan(int span_type, void* detail, void* userdata);
    static int ProcessText(int text_type, const char* text, unsigned size, void* userdata);
    
    struct RenderContext {
        std::string* output;
        bool plain_text_mode;
    };
};

}
