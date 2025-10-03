#pragma once
#include <string>
#include <vector>
#include <memory>
#include <regex>

namespace ShinoEditor {

enum class BlockType {
    PARAGRAPH,
    HEADER,
    CODE_FENCE,
    QUOTE
};

struct Block {
    BlockType type;
    int start_line;
    int end_line;
    int level = 0;  // For headers (1-6) or quote depth
    bool is_folded = false;
    std::string header_text;  // For display when folded
    
    Block(BlockType t, int start, int end) 
        : type(t), start_line(start), end_line(end) {}
};

class BlockModel {
public:
    // アプリ側の行バッファを参照で保持（コピーしない）
    explicit BlockModel(std::vector<std::string>& lines);
    
    // Parse lines and detect blocks
    void ParseBlocks();
    
    // Block manipulation
    void ToggleFold(int line_number);
    bool MoveBlockUp(int line_number);
    bool MoveBlockDown(int line_number);
    
    // Get block at line
    std::shared_ptr<Block> GetBlockAt(int line_number) const;
    
    // Get all blocks
    const std::vector<std::shared_ptr<Block>>& GetBlocks() const { return blocks_; }
    
    // Get visible lines (considering folding)
    std::vector<std::string> GetVisibleLines() const;
    // 可視行インデックス -> 実行行インデックスのマップ
    std::vector<int> GetVisibleLineIndices() const;
    
    // Update with new lines (内容は外部で更新済みなので再解析のみ)
    void UpdateLines();

private:
    // App::lines_ を参照で保持して、二重管理・余計なメモリ消費を防ぐ
    std::vector<std::string>& lines_;
    std::vector<std::shared_ptr<Block>> blocks_;
    
    // キャッシュとバッファ
    mutable std::vector<std::string> visible_lines_cache_;
    mutable std::vector<int> visible_indices_cache_;
    mutable bool cache_valid_ = false;
    
    // 正規表現パターンのキャッシュ
    static const std::regex header_pattern_;
    static const std::regex header_extract_pattern_;

    // Helper methods
    bool IsHeaderLine(const std::string& line, int& level) const;
    bool IsCodeFenceStart(const std::string& line) const;
    bool IsQuoteLine(const std::string& line) const;
    std::string ExtractHeaderText(const std::string& line) const;

    // 可視ビュー構築ヘルパ
    void BuildVisibleView(std::vector<std::string>& out_lines,
                          std::vector<int>& out_indices) const;
};

}
