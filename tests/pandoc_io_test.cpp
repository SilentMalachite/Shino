#include "test_framework.h"
#include "pandoc_io.h"
#include <filesystem>

using namespace ShinoEditor;
namespace fs = std::filesystem;

TEST(PandocAvailability) {
    bool available = PandocIO::IsPandocAvailable();
    // Note: This test may fail if pandoc is not installed. That's expected.
    ASSERT_TRUE(available);
}

TEST(ImportDocx_InvalidFile) {
    auto result = PandocIO::ImportDocx("/nonexistent/file.docx");
    ASSERT_FALSE(result.has_value());
}

TEST(ExportDocx_InvalidPath) {
    bool result = PandocIO::ExportDocx("# Test", "/nonexistent/path/file.docx");
    ASSERT_FALSE(result);
}

TEST(ExportImport_RoundTrip) {
    // Only run if pandoc is available
    if (!PandocIO::IsPandocAvailable()) {
        std::cout << "Skipping pandoc round-trip test (pandoc not available)\n";
        return;
    }

    // Create temp dir for test files
    auto temp_dir = test_utils::create_temp_dir("pandoc_test");
    auto docx_path = temp_dir / "test.docx";
    
    // Test content with various Markdown features
    std::string markdown = 
        "# Test Document\n"
        "\n"
        "This is a paragraph with **bold** and *italic* text.\n"
        "\n"
        "- List item 1\n"
        "- List item 2\n"
        "  - Nested item\n"
        "\n"
        "## Section 2\n"
        "\n"
        "```\nCode block\n```\n"
        "\n"
        "> Blockquote\n"
        "\n"
        "### 日本語セクション\n"
        "\n"
        "UTF-8テキストのテスト\n";

    // Export to DOCX
    bool export_ok = PandocIO::ExportDocx(markdown, docx_path.string());
    ASSERT_TRUE(export_ok);
    ASSERT_TRUE(fs::exists(docx_path));

    // Import back
    auto imported = PandocIO::ImportDocx(docx_path.string());
    ASSERT_TRUE(imported.has_value());
    
    // Basic content checks
    const auto& content = imported.value();
    ASSERT_TRUE(content.find("Test Document") != std::string::npos);
    ASSERT_TRUE(content.find("Section 2") != std::string::npos);
    ASSERT_TRUE(content.find("日本語セクション") != std::string::npos);
    ASSERT_TRUE(content.find("UTF-8テキスト") != std::string::npos);
    
    // Cleanup
    test_utils::cleanup_temp_dir(temp_dir);
}

int main() {
    return run_all_tests();
}