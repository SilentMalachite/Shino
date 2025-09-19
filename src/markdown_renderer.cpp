#include "markdown_renderer.h"
#ifdef HAVE_MD4C
#include <md4c-html.h>
// 後方互換用: 一部の md4c には MD_FLAG_TASKLISTS が存在しない
#ifndef MD_FLAG_TASKLISTS
#define MD_FLAG_TASKLISTS 0
#endif
#endif
#include <sstream>
#include <regex>

namespace ShinoEditor {

MarkdownRenderer::MarkdownRenderer() = default;

MarkdownRenderer::~MarkdownRenderer() = default;

std::string MarkdownRenderer::RenderToHtml(const std::string& markdown) const {
#ifdef HAVE_MD4C
    std::string html_output;
    RenderContext context{&html_output, false};
    
    // MD4C callback functions
    auto process_output = [](const MD_CHAR* text, MD_SIZE size, void* userdata) {
        RenderContext* ctx = static_cast<RenderContext*>(userdata);
        ctx->output->append(text, size);
    };
    
    // md4c のパーサーフラグを設定（存在しないフラグは 0 として扱う）
    unsigned parser_flags = MD_FLAG_TABLES | MD_FLAG_STRIKETHROUGH | MD_FLAG_TASKLISTS;
    int result = md_html(markdown.c_str(), (MD_SIZE)markdown.size(),
                        process_output, &context,
                        parser_flags,
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
    // 行単位に処理して ^ アンカーを正しく機能させる
    std::istringstream in(markdown);
    std::ostringstream out;
    std::string line;
    bool first = true;

    const std::regex header_re(R"(^\s*#{1,6}\s*)");
    const std::regex bold_re(R"(\*\*([^*]+)\*\*)");
    const std::regex italic_re(R"(\*([^*]+)\*)");
    const std::regex link_re(R"(\[([^\]]+)\]\([^)]+\))");
    const std::regex code_re(R"(`([^`]+)`)");
    const std::regex quote_re(R"(^\s*>\s*)");

    while (std::getline(in, line)) {
        line = std::regex_replace(line, header_re, "");
        line = std::regex_replace(line, bold_re, "$1");
        line = std::regex_replace(line, italic_re, "$1");
        line = std::regex_replace(line, link_re, "$1");
        line = std::regex_replace(line, code_re, "$1");
        line = std::regex_replace(line, quote_re, "");
        if (!first) out << '\n';
        out << line;
        first = false;
    }

    return out.str();
}

bool MarkdownRenderer::IsAvailable() {
#ifdef HAVE_MD4C
    return true;
#else
    return false;
#endif
}

}
