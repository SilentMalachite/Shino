#include "test_framework.h"
#include "app_test_helper.h"
#include "tui_bindings.h"
#include <filesystem>
#include <fstream>

using namespace ShinoEditor;
namespace fs = std::filesystem;

TEST(App_OpenNonexistentFile) {
    test::AppTestHelper helper;
    int result = helper.RunWithTimeout("/nonexistent/file.md");
    ASSERT_NE(result, 0); // Should fail
}

TEST(App_BasicEditing) {
    auto temp_file = test_utils::create_temp_file("");
    test::AppTestHelper helper;
    
    // Start app
    helper.RunWithTimeout(temp_file.string());
    
    // Type some text
    helper.SendKeys({"H", "e", "l", "l", "o"});
    helper.SendSpecialKey(ftxui::Event::Return); // Save edit
    
    // Save file
    helper.SendControlKey(TUIBindings::CTRL_O);
    
    // Verify file content
    std::ifstream file(temp_file);
    std::string content;
    std::getline(file, content);
    ASSERT_EQ(content, "Hello");
    
    fs::remove(temp_file);
}

TEST(App_Search) {
    test::AppTestHelper helper;
    
    // Type some lines
    helper.SendKeys({"L", "i", "n", "e", " ", "1"});
    helper.SendSpecialKey(ftxui::Event::Return);
    helper.SendKeys({"L", "i", "n", "e", " ", "2"});
    helper.SendSpecialKey(ftxui::Event::Return);
    helper.SendKeys({"T", "e", "s", "t", " ", "l", "i", "n", "e"});
    helper.SendSpecialKey(ftxui::Event::Return);
    
    // Open search
    helper.SendControlKey(TUIBindings::CTRL_W);
    
    // Search for "line"
    helper.SendKeys({"l", "i", "n", "e"});
    helper.SendSpecialKey(ftxui::Event::Return);
    
    // Navigate matches
    helper.SendKeys({"n"}); // Next match
    helper.SendKeys({"p"}); // Previous match
}

TEST(App_BlockOperations) {
    test::AppTestHelper helper;
    
    // Create a markdown document with blocks
    helper.SendKeys({"#", " ", "H", "e", "a", "d", "e", "r"});
    helper.SendSpecialKey(ftxui::Event::Return);
    helper.SendKeys({"P", "a", "r", "a", "g", "r", "a", "p", "h"});
    helper.SendSpecialKey(ftxui::Event::Return);
    
    // Toggle fold
    helper.SendControlKey(TUIBindings::CTRL_J);
    
    // Move block
    helper.SendSpecialKey(ftxui::Event::PageDown);
}

TEST(App_Preview) {
    test::AppTestHelper helper;
    
    // Create some markdown content
    helper.SendKeys({"#", " ", "T", "i", "t", "l", "e"});
    helper.SendSpecialKey(ftxui::Event::Return);
    
    // Toggle preview
    helper.SendControlKey(TUIBindings::CTRL_P);
    
    // Toggle back
    helper.SendControlKey(TUIBindings::CTRL_P);
}

TEST(App_Help) {
    test::AppTestHelper helper;
    
    // Show help
    helper.SendControlKey(TUIBindings::CTRL_G);
    
    // Hide help
    helper.SendControlKey(TUIBindings::CTRL_G);
}

TEST(App_SaveAs) {
    auto temp_dir = test_utils::create_temp_dir("app_test");
    auto test_file = temp_dir / "test.md";
    test::AppTestHelper helper;
    
    // Type some content
    helper.SendKeys({"T", "e", "s", "t"});
    helper.SendSpecialKey(ftxui::Event::Return);
    
    // Save (will prompt for filename)
    helper.SendControlKey(TUIBindings::CTRL_O);
    
    // Enter filename
    for (char c : test_file.string()) {
        helper.SendKeys({std::string(1, c)});
    }
    helper.SendSpecialKey(ftxui::Event::Return);
    
    // Verify file was created
    ASSERT_TRUE(fs::exists(test_file));
    
    test_utils::cleanup_temp_dir(temp_dir);
}

TEST(App_Japanese) {
    test::AppTestHelper helper;
    
    // Type Japanese text (UTF-8)
    std::vector<std::string> jp_text = {"テ", "ス", "ト"};
    helper.SendKeys(jp_text);
    helper.SendSpecialKey(ftxui::Event::Return);
    
    // Toggle preview to ensure rendering works
    helper.SendControlKey(TUIBindings::CTRL_P);
}

int main() {
    return run_all_tests();
}