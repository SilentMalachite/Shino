#include "tui_bindings.h"

namespace ShinoEditor {

std::string TUIBindings::GetHelpLine() {
    return "^O 保存  ^X 終了  ^W 検索  ^G ヘルプ  ^J フォールド  ^P プレビュー  ^I ImportDOCX  ^E ExportDOCX";
}

std::vector<KeyBinding> TUIBindings::GetAllBindings() {
    return {
        {"Ctrl+O", "ファイルを保存 (Write Out)"},
        {"Ctrl+X", "エディタを終了"},
        {"Ctrl+W", "テキストを検索"},
        {"Ctrl+G", "ヘルプを表示/非表示"},
        {"Ctrl+J", "現在のブロックを折り畳み/展開"},
        {"Page Up/Down", "現在のブロックを上下に移動"},
        {"Ctrl+P", "プレビュー表示を切り替え"},
        {"Ctrl+I", "DOCX ファイルをインポート (pandoc必須)"},
        {"Ctrl+E", "DOCX ファイルにエクスポート (pandoc必須)"},
        {"↑/↓", "カーソルを上下に移動"},
        {"Enter", "新しい行を挿入"},
        {"Delete/Backspace", "現在の行を削除"},
        {"文字キー", "編集モードに入る"},
        {"Enter (編集中)", "編集を保存"},
        {"Esc (編集中)", "編集をキャンセル"}
    };
}

}
