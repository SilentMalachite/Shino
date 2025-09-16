#include "app.h"
#include "tui_bindings.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/string.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace ftxui;

namespace ShinoEditor {

namespace {
// UTF-8の末尾1コードポイントを安全に削除
void Utf8PopBack(std::string& s) {
    if (s.empty()) return;
    // 末尾から先頭バイトを探す
    size_t i = s.size();
    // 最後のバイト位置
    if (i == 0) return;
    // 1バイト戻しつつ、先頭バイトに当たるまで戻る
    do {
        --i;
        unsigned char c = static_cast<unsigned char>(s[i]);
        if ((c & 0xC0) != 0x80) { // 先頭バイト（10xxxxxx 以外）
            s.erase(i);
            return;
        }
    } while (i > 0);
    // 念のため全消去（異常なバイト列）
    s.clear();
}
}

App::App() : screen_(ScreenInteractive::Fullscreen()) {
    block_model_ = std::make_unique<BlockModel>(lines_);
    renderer_ = std::make_unique<MarkdownRenderer>();
    main_component_ = CreateMainComponent();
}

App::~App() {}

int App::Run(const std::string& filename) {
    if (!filename.empty()) {
        if (!LoadFile(filename)) {
            std::cerr << "Failed to load file: " << filename << std::endl;
            return 1;
        }
    }
    
    screen_.Loop(main_component_);
    return 0;
}

bool App::LoadFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        return false;
    }
    
    lines_.clear();
    std::string line;
    while (std::getline(file, line)) {
        lines_.push_back(line);
    }
    
    filename_ = filename;
    modified_ = false;
    UpdateBlockModel();
    return true;
}

bool App::SaveFile() {
    if (filename_.empty()) {
        SetStatusMessage("No filename specified");
        return false;
    }
    
    return SaveFileAs(filename_);
}

bool App::SaveFileAs(const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        SetStatusMessage("Failed to save file: " + filename);
        return false;
    }
    
    for (const auto& line : lines_) {
        file << line << '\n';
    }
    
    filename_ = filename;
    modified_ = false;
    SetStatusMessage("Saved: " + filename);
    return true;
}

void App::ToggleBlockFold() {
    int real = VisibleToRealIndex(current_line_);
    if (real >= 0) {
        auto blk = block_model_->GetBlockAt(real);
        block_model_->ToggleFold(real);
        // 折りたたみ直後に不可視化された場合は、可視範囲へ自動補正
        int vi = RealToVisibleIndex(real);
        if (vi >= 0) {
            current_line_ = vi;
        } else if (blk) {
            int header_vi = RealToVisibleIndex(blk->start_line);
            if (header_vi >= 0) current_line_ = header_vi;
        }
        // 最終クランプ
        auto vis = GetVisibleEditorLines();
        if (vis.empty()) {
            current_line_ = 0;
        } else if (current_line_ >= static_cast<int>(vis.size())) {
            current_line_ = static_cast<int>(vis.size()) - 1;
        }
    }
    SetStatusMessage("Toggled fold");
}

void App::MoveBlockUp() {
    int real = VisibleToRealIndex(current_line_);
    if (real >= 0 && block_model_->MoveBlockUp(real)) {
        modified_ = true;
        SetStatusMessage("Block moved up");
    } else {
        SetStatusMessage("Cannot move block up");
    }
}

void App::MoveBlockDown() {
    int real = VisibleToRealIndex(current_line_);
    if (real >= 0 && block_model_->MoveBlockDown(real)) {
        modified_ = true;
        SetStatusMessage("Block moved down");
    } else {
        SetStatusMessage("Cannot move block down");
    }
}

void App::TogglePreview() {
    show_preview_ = !show_preview_;
    SetStatusMessage(show_preview_ ? "Preview enabled" : "Preview disabled");
}

void App::ToggleHelp() {
    show_help_ = !show_help_;
    help_tab_index_ = show_help_ ? 1 : 0;
}

void App::ShowSearch() {
    SetStatusMessage("Search not implemented yet");
}

void App::ImportDocx() {
    if (!PandocIO::IsPandocAvailable()) {
        SetStatusMessage("Pandoc not available");
        return;
    }
    
    std::string docx_path = PromptForFilename("Enter DOCX filename to import: ");
    if (docx_path.empty()) {
        SetStatusMessage("Import cancelled");
        return;
    }
    
    auto result = PandocIO::ImportDocx(docx_path);
    if (result) {
        // Parse the imported markdown into lines
        lines_.clear();
        std::istringstream ss(*result);
        std::string line;
        while (std::getline(ss, line)) {
            lines_.push_back(line);
        }
        UpdateBlockModel();
        modified_ = true;
        current_line_ = 0;
        SetStatusMessage("DOCX imported successfully");
    } else {
        SetStatusMessage("Failed to import DOCX file");
    }
}

void App::ExportDocx() {
    if (!PandocIO::IsPandocAvailable()) {
        SetStatusMessage("Pandoc not available");
        return;
    }
    
    std::string docx_path = PromptForFilename("Enter DOCX filename to export: ");
    if (docx_path.empty()) {
        SetStatusMessage("Export cancelled");
        return;
    }
    
    // Convert lines to markdown string
    std::ostringstream ss;
    for (const auto& line : lines_) {
        ss << line << "\n";
    }
    
    if (PandocIO::ExportDocx(ss.str(), docx_path)) {
        SetStatusMessage("DOCX exported successfully");
    } else {
        SetStatusMessage("Failed to export DOCX file");
    }
}

Component App::CreateMainComponent() {
    editor_component_ = CreateEditorComponent();
    preview_component_ = CreatePreviewComponent();
    
    auto content = Container::Horizontal({
        editor_component_,
        preview_component_
    });
    
    auto help_component = CreateHelpComponent();
    auto status_component = CreateStatusComponent();
    
    auto main_layout = Container::Vertical({
        content,
        status_component
    });
    
    // Add help overlay if needed
    help_tab_index_ = show_help_ ? 1 : 0;
    auto with_help = Container::Tab({
        main_layout,
        help_component
    }, &help_tab_index_);
    
    return CatchEvent(with_help, [this](Event event) {
        return HandleKeyPress(event);
    });
}

Component App::CreateEditorComponent() {
    return Renderer([this] {
        auto visible_lines = GetVisibleEditorLines();
        
        Elements elements;
        for (int i = 0; i < static_cast<int>(visible_lines.size()); ++i) {
            std::string line_content;
            if (editing_mode_ && i == current_line_) {
                line_content = current_input_ + "_"; // Show cursor
            } else {
                line_content = i < static_cast<int>(visible_lines.size()) ? visible_lines[i] : "";
            }
            
            auto line_element = text(to_wstring(line_content));
            if (i == current_line_) {
                line_element = line_element | bgcolor(editing_mode_ ? Color::Green : Color::Blue);
            }
            elements.push_back(line_element);
        }
        
        // Add some padding if no lines
        if (elements.empty()) {
            elements.push_back(text("[Empty file - press any key to start editing]"));
        }
        
        return vbox(elements) | border | flex;
    });
}

Component App::CreatePreviewComponent() {
    return Renderer([this] {
        if (!show_preview_) {
            return text("");
        }
        
        std::string content = GetPreviewContent();
        Elements elems;
        elems.push_back(text(L"Preview") | bold);
        elems.push_back(separator());
        elems.push_back(paragraph(content));
        return vbox(elems) | border | flex;
    });
}

Component App::CreateHelpComponent() {
    return Renderer([this] {
        auto bindings = TUIBindings::GetAllBindings();
        
        Elements help_elements;
        help_elements.push_back(text(L"ShinoEditor Help") | bold | center);
        help_elements.push_back(separator());
        
        for (const auto& binding : bindings) {
            help_elements.push_back(
                hbox({
                    text(to_wstring(binding.key)) | bold | size(WIDTH, EQUAL, 15),
                    text(L" : "),
                    text(to_wstring(binding.description))
                })
            );
        }
        
        help_elements.push_back(separator());
        help_elements.push_back(text("Press Ctrl+G to close help") | center);
        
        return vbox(help_elements) | border | center;
    });
}

Component App::CreateStatusComponent() {
    return Renderer([this] {
        std::string filename_display = filename_.empty() ? "[New File]" : filename_;
        std::string modified_indicator = modified_ ? "*" : "";
        
        return hbox({
            text(to_wstring(TUIBindings::GetHelpLine())) | flex,
            separator(),
            text(to_wstring(filename_display + modified_indicator)),
            separator(),
            text(to_wstring(status_message_))
        }) | border;
    });
}

bool App::HandleKeyPress(Event event) {
    // Handle special key combinations (Ctrl keys using character codes)
    if (event == Event::Character('\x0F')) { // Ctrl+O
        SaveFile();
        return true;
    }
    
    if (event == Event::Character('\x18')) { // Ctrl+X
        screen_.ExitLoopClosure()();
        return true;
    }
    
    if (event == Event::Character('\x17')) { // Ctrl+W
        ShowSearch();
        return true;
    }
    
    if (event == Event::Character('\x07')) { // Ctrl+G
        ToggleHelp();
        return true;
    }
    
    if (event == Event::Character('\x0A')) { // Ctrl+J
        ToggleBlockFold();
        return true;
    }
    
    if (event == Event::Character('\x10')) { // Ctrl+P
        TogglePreview();
        return true;
    }
    
    if (event == Event::Character('\x09')) { // Ctrl+I (Tab)
        ImportDocx();
        return true;
    }
    
    if (event == Event::Character('\x05')) { // Ctrl+E
        ExportDocx();
        return true;
    }
    
    // Handle text editing keys
    if (event == Event::Return) {
        if (editing_mode_) {
            // Save current edit
            int real = VisibleToRealIndex(current_line_);
            if (real >= 0 && real < static_cast<int>(lines_.size())) {
                lines_[real] = current_input_;
            } else {
                lines_.push_back(current_input_);
            }
            modified_ = true;
            UpdateBlockModel();
            editing_mode_ = false;
            current_input_ = "";
            SetStatusMessage("Line saved");
            return true;
        } else {
            // Insert new line
            InsertLine();
            return true;
        }
    }
    
    if (event == Event::Escape) {
        if (editing_mode_) {
            ExitEditMode();
            return true;
        }
    }
    
    if (event == Event::Delete || event == Event::Backspace) {
        if (editing_mode_) {
            Utf8PopBack(current_input_);
            return true;
        } else {
            DeleteLine();
            return true;
        }
    }
    
    // いずれかの入力文字で編集モードへ（UTF-8対応）
    if (!editing_mode_ && event.is_character() && !event.character().empty()) {
        EnterEditMode();
        current_input_ = event.character();
        return true;
    }
    
    // Handle typing in edit mode
    if (editing_mode_ && event.is_character()) {
        current_input_ += event.character();
        return true;
    }
    
    // Handle Alt + arrow keys for block movement
    // Use Page Up/Down as alternative (terminal-compatible)
    if (event == Event::PageUp) {
        MoveBlockUp();
        return true;
    }
    
    if (event == Event::PageDown) {
        MoveBlockDown();
        return true;
    }
    
    // Handle basic navigation
    if (event == Event::ArrowUp) {
        if (current_line_ > 0) {
            current_line_--;
        }
        return true;
    }
    
    if (event == Event::ArrowDown) {
        auto visible_lines = GetVisibleEditorLines();
        if (current_line_ < static_cast<int>(visible_lines.size()) - 1) {
            current_line_++;
        }
        return true;
    }
    
    return false;
}

void App::UpdateBlockModel() {
    block_model_->UpdateLines();
}

void App::SetStatusMessage(const std::string& message) {
    status_message_ = message;
}

void App::InsertLine() {
    int real = VisibleToRealIndex(current_line_);
    if (real < 0) {
        lines_.push_back("");
    } else {
        lines_.insert(lines_.begin() + real + 1, "");
        current_line_++; // 可視上は1つ下へ
    }
    modified_ = true;
    UpdateBlockModel();
    SetStatusMessage("Line inserted");
}

void App::DeleteLine() {
    int real = VisibleToRealIndex(current_line_);
    if (!lines_.empty() && real >= 0 && real < static_cast<int>(lines_.size())) {
        lines_.erase(lines_.begin() + real);
        modified_ = true;
        UpdateBlockModel();
        // 可視行数に合わせてカーソルをクランプ
        auto vis = GetVisibleEditorLines();
        if (current_line_ >= static_cast<int>(vis.size()) && current_line_ > 0) {
            current_line_ = static_cast<int>(vis.size()) - 1;
        }
        SetStatusMessage("Line deleted");
    }
}

void App::EnterEditMode() {
    editing_mode_ = true;
    int real = VisibleToRealIndex(current_line_);
    current_input_ = (real >= 0 && real < static_cast<int>(lines_.size())) ? lines_[real] : "";
    SetStatusMessage("Editing mode - Press Enter to save, Esc to cancel");
}

void App::ExitEditMode() {
    editing_mode_ = false;
    current_input_ = "";
    SetStatusMessage("Edit cancelled");
}

std::string App::PromptForFilename(const std::string& prompt) {
    // Simple implementation - in a real app you'd want a proper input dialog
    SetStatusMessage(prompt + "[Not implemented - using default]");
    return "example.docx"; // Placeholder
}

std::vector<std::string> App::GetVisibleEditorLines() const {
    return block_model_->GetVisibleLines();
}

std::string App::GetPreviewContent() const {
    std::stringstream ss;
    for (const auto& line : lines_) {
        ss << line << "\n";
    }
    
    // Render to HTML first, then display as text (per AGENT.md spec)
    std::string html = renderer_->RenderToHtml(ss.str());
    return html.empty() ? renderer_->RenderToText(ss.str()) : html;
}

int App::VisibleToRealIndex(int visible_index) const {
    auto indices = block_model_->GetVisibleLineIndices();
    if (visible_index < 0 || visible_index >= static_cast<int>(indices.size())) return -1;
    return indices[visible_index];
}

int App::RealToVisibleIndex(int real_index) const {
    auto indices = block_model_->GetVisibleLineIndices();
    for (int i = 0; i < static_cast<int>(indices.size()); ++i) {
        if (indices[i] == real_index) return i;
    }
    // 折りたたみ内部の場合はブロック先頭へ寄せる
    auto blk = block_model_->GetBlockAt(real_index);
    if (blk) {
        for (int i = 0; i < static_cast<int>(indices.size()); ++i) {
            if (indices[i] == blk->start_line) return i;
        }
    }
    // フォールバック: 最後の可視行
    return indices.empty() ? -1 : static_cast<int>(indices.size()) - 1;
}

}
