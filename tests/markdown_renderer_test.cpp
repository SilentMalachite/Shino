#include "test_framework.h"
#include "markdown_renderer.h"

using namespace ShinoEditor;

TEST(RenderToText_Basic) {
    auto renderer = std::make_unique<MarkdownRenderer>();
    std::string md = "# Header\nParagraph\n";
    auto result = renderer->RenderToText(md);
    ASSERT_TRUE(result.find("Header") != std::string::npos);
    ASSERT_TRUE(result.find("Paragraph") != std::string::npos);
}

TEST(RenderToText_Lists) {
    auto renderer = std::make_unique<MarkdownRenderer>();
    std::string md = "- Item 1\n- Item 2\n  - Nested\n";
    auto result = renderer->RenderToText(md);
    ASSERT_TRUE(result.find("Item 1") != std::string::npos);
    ASSERT_TRUE(result.find("Item 2") != std::string::npos);
    ASSERT_TRUE(result.find("Nested") != std::string::npos);
}

TEST(RenderToText_CodeBlocks) {
    auto renderer = std::make_unique<MarkdownRenderer>();
    std::string md = "```\ncode\n```\n";
    auto result = renderer->RenderToText(md);
    ASSERT_TRUE(result.find("code") != std::string::npos);
}

TEST(RenderToText_BlockQuotes) {
    auto renderer = std::make_unique<MarkdownRenderer>();
    std::string md = "> Quote\n>> Nested\n";
    auto result = renderer->RenderToText(md);
    ASSERT_TRUE(result.find("Quote") != std::string::npos);
    ASSERT_TRUE(result.find("Nested") != std::string::npos);
}

TEST(RenderToText_InlineElements) {
    auto renderer = std::make_unique<MarkdownRenderer>();
    std::string md = "**bold** *italic* `code`\n";
    auto result = renderer->RenderToText(md);
    ASSERT_TRUE(result.find("bold") != std::string::npos);
    ASSERT_TRUE(result.find("italic") != std::string::npos);
    ASSERT_TRUE(result.find("code") != std::string::npos);
}

TEST(RenderToHtml_Basic) {
    auto renderer = std::make_unique<MarkdownRenderer>();
    std::string md = "# Header\nParagraph\n";
    auto result = renderer->RenderToHtml(md);
    ASSERT_TRUE(result.find("<h1>") != std::string::npos);
    ASSERT_TRUE(result.find("Header") != std::string::npos);
    ASSERT_TRUE(result.find("<p>") != std::string::npos);
}

TEST(RenderToHtml_Lists) {
    auto renderer = std::make_unique<MarkdownRenderer>();
    std::string md = "- Item 1\n- Item 2\n";
    auto result = renderer->RenderToHtml(md);
    ASSERT_TRUE(result.find("<ul>") != std::string::npos);
    ASSERT_TRUE(result.find("<li>") != std::string::npos);
    ASSERT_TRUE(result.find("Item 1") != std::string::npos);
}

TEST(RenderToHtml_Japanese) {
    auto renderer = std::make_unique<MarkdownRenderer>();
    std::string md = "# 見出し\n本文\n";
    auto result = renderer->RenderToHtml(md);
    ASSERT_TRUE(result.find("見出し") != std::string::npos);
    ASSERT_TRUE(result.find("本文") != std::string::npos);
}

TEST(RenderToHtml_Empty) {
    auto renderer = std::make_unique<MarkdownRenderer>();
    auto result = renderer->RenderToHtml("");
    ASSERT_TRUE(result.empty());
}

int main() {
    return run_all_tests();
}