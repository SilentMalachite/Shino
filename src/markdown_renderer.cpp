#include "markdown_renderer.h"
#ifdef HAVE_MD4C
#include <md4c-html.h>
#endif
#include <sstream>
#include <regex>

namespace ShinoEditor {

MarkdownRenderer::MarkdownRenderer() {}

MarkdownRenderer::~MarkdownRenderer() {}

std::string MarkdownRenderer::RenderToHtml(const std::string& markdown) const {
#ifdef HAVE_MD4C
    std::string html_output;
    RenderContext context{&html_output, false};
    
    // MD4C callback functions
    auto process_output = [](const MD_CHAR* text, MD_SIZE size, void* userdata) {
        RenderContext* ctx = static_cast<RenderContext*>(userdata);
        ctx->output->append(text, size);
    };
    
    int result = md_html(markdown.c_str(), markdown.length(),
                        process_output, &context,
                        MD_FLAG_TABLES | MD_FLAG_FENCEDCODE | MD_FLAG_STRIKETHROUGH,
                        0);
    
    if (result != 0) {
        return "<p>Error rendering markdown</p>";
    }
    
    return html_output;
#else
    // Fallback: simple HTML conversion
    return "<pre>" + markdown + "</pre>";
#endif
}

std::string MarkdownRenderer::RenderToText(const std::string& markdown) const {
    // Simple text rendering - strip markdown syntax
    std::string text = markdown;
    
    // Remove headers
    text = std::regex_replace(text, std::regex(R"(^#{1,6}\s*)"), "");
    
    // Remove bold/italic
    text = std::regex_replace(text, std::regex(R"(\*\*([^*]+)\*\*)"), "$1");
    text = std::regex_replace(text, std::regex(R"(\*([^*]+)\*)"), "$1");
    
    // Remove links
    text = std::regex_replace(text, std::regex(R"(\[([^\]]+)\]\([^)]+\))"), "$1");
    
    // Remove code markers
    text = std::regex_replace(text, std::regex("`([^`]+)`"), "$1");
    
    // Remove quote markers
    text = std::regex_replace(text, std::regex(R"(^>\s*)"), "");
    
    return text;
}

bool MarkdownRenderer::IsAvailable() {
#ifdef HAVE_MD4C
    return true;
#else
    return false;
#endif
}

}
