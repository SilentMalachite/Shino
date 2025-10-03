#pragma once
#include "block_model.h"
#include "markdown_renderer.h"
#include "pandoc_io.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace ShinoEditor {

// Forward declare for friend access
namespace test { class AppTestHelper; }

class App {
    friend class test::AppTestHelper;
public:
    App();
    ~App();
    
    // Run the application
    int Run(const std::string& filename = "");
    
private:
    // UI state
    std::string filename_;
    std::vector<std::string> lines_;
    std::unique_ptr<BlockModel> block_model_;
    std::unique_ptr<MarkdownRenderer> renderer_;
    
    // UI components
    ftxui::ScreenInteractive screen_;
    ftxui::Component main_component_;
    ftxui::Component editor_component_;
    ftxui::Component preview_component_;
    
    // Application state
    bool show_preview_ = false;
    bool show_help_ = false;
    bool modified_ = false;
    int current_line_ = 0;
    int scroll_offset_ = 0;
    int help_tab_index_ = 0;
    bool editing_mode_ = false;
    std::string current_input_;

    // Search state
    bool show_search_ = false;
    std::string search_query_;
    std::vector<int> search_matches_; // real line indices
    int current_match_ = -1; // index into search_matches_

    std::string status_message_;
    
    // Filename prompt dialog state
    bool show_filename_prompt_ = false;
    std::string filename_prompt_message_;
    std::string filename_prompt_text_;
    std::function<void(const std::string&)> filename_prompt_callback_;
    
    // File operations
    bool LoadFile(const std::string& filename);
    bool SaveFile();
    bool SaveFileAs(const std::string& filename);
    
    // Editor operations
    void ToggleBlockFold();
    void MoveBlockUp();
    void MoveBlockDown();
    void TogglePreview();
    void ToggleHelp();
    void ShowSearch();
    void FindMatches(const std::string& query);
    void GotoNextMatch();
    void GotoPrevMatch();
    void HideSearch();
    void ImportDocx();
    void ExportDocx();
    void InsertLine();
    void DeleteLine();
    void EnterEditMode();
    void ExitEditMode();
    std::string PromptForFilename(const std::string& prompt);
    
    // UI builders
    ftxui::Component CreateMainComponent();
    ftxui::Component CreateEditorComponent();
    ftxui::Component CreatePreviewComponent();
    ftxui::Component CreateHelpComponent();
    ftxui::Component CreateStatusComponent();
    ftxui::Component CreateFilenamePromptComponent();
    ftxui::Component CreateSearchPromptComponent();
    
    // Filename prompt operations
    void ShowFilenamePrompt(const std::string& message, const std::string& default_value, std::function<void(const std::string&)> callback);
    void HideFilenamePrompt();
    void ConfirmFilenamePrompt();
    
    // Event handlers
    bool HandleKeyPress(const ftxui::Event& event);
    
    // Helper methods
    void UpdateBlockModel();
    void SetStatusMessage(const std::string& message);
    std::vector<std::string> GetVisibleEditorLines() const;
    std::string GetPreviewContent() const;

    // 可視行インデックス -> 実行行インデックス 変換
    int VisibleToRealIndex(int visible_index) const;
    // 実行行インデックス -> 可視行インデックス 変換（見つからない場合は折りたたみ先頭などに寄せる）
    int RealToVisibleIndex(int real_index) const;
};

}
