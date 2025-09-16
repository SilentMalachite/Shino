#include "block_model.h"
#include <algorithm>
#include <regex>

namespace ShinoEditor {

BlockModel::BlockModel(std::vector<std::string>& lines)
    : lines_(lines) {
    ParseBlocks();
}

void BlockModel::ParseBlocks() {
    blocks_.clear();
    
    if (lines_.empty()) return;
    
    bool in_code_fence = false;
    int code_fence_start = -1;
    int paragraph_start = -1; // 通常行の開始

    for (int i = 0; i < static_cast<int>(lines_.size()); ++i) {
        const std::string& line = lines_[i];

        // コードフェンス開始/終了
        if (IsCodeFenceStart(line)) {
            if (!in_code_fence) {
                // 途中の段落を確定
                if (paragraph_start != -1) {
                    auto block = std::make_shared<Block>(BlockType::PARAGRAPH, paragraph_start, i - 1);
                    block->header_text = "[段落]";
                    blocks_.push_back(block);
                    paragraph_start = -1;
                }
                in_code_fence = true;
                code_fence_start = i;
            } else {
                in_code_fence = false;
                auto block = std::make_shared<Block>(BlockType::CODE_FENCE, code_fence_start, i);
                block->header_text = "[コードブロック]";
                blocks_.push_back(block);
            }
            continue;
        }

        if (in_code_fence) continue;

        // 見出し
        int level = 0;
        if (IsHeaderLine(line, level)) {
            if (paragraph_start != -1) {
                auto block = std::make_shared<Block>(BlockType::PARAGRAPH, paragraph_start, i - 1);
                block->header_text = "[段落]";
                blocks_.push_back(block);
                paragraph_start = -1;
            }
            auto block = std::make_shared<Block>(BlockType::HEADER, i, i);
            block->level = level;
            block->header_text = ExtractHeaderText(line);
            blocks_.push_back(block);
            continue;
        }

        // 引用ブロック
        if (IsQuoteLine(line)) {
            if (paragraph_start != -1) {
                auto block = std::make_shared<Block>(BlockType::PARAGRAPH, paragraph_start, i - 1);
                block->header_text = "[段落]";
                blocks_.push_back(block);
                paragraph_start = -1;
            }
            int quote_start = i;
            int quote_end = i;
            while (quote_end + 1 < static_cast<int>(lines_.size()) && IsQuoteLine(lines_[quote_end + 1])) {
                quote_end++;
            }
            auto block = std::make_shared<Block>(BlockType::QUOTE, quote_start, quote_end);
            block->header_text = "[引用ブロック]";
            blocks_.push_back(block);
            i = quote_end;
            continue;
        }

        // 通常行（段落）
        if (paragraph_start == -1) {
            paragraph_start = i;
        }
    }

    // 未クローズのコードフェンス
    if (in_code_fence) {
        auto block = std::make_shared<Block>(BlockType::CODE_FENCE, code_fence_start, static_cast<int>(lines_.size()) - 1);
        block->header_text = "[コードブロック]";
        blocks_.push_back(block);
    }

    // ファイル末尾まで続く段落
    if (!in_code_fence && paragraph_start != -1) {
        auto block = std::make_shared<Block>(BlockType::PARAGRAPH, paragraph_start, static_cast<int>(lines_.size()) - 1);
        block->header_text = "[段落]";
        blocks_.push_back(block);
    }
}

void BlockModel::ToggleFold(int line_number) {
    auto block = GetBlockAt(line_number);
    if (block && block->start_line != block->end_line) {
        block->is_folded = !block->is_folded;
    }
}

bool BlockModel::MoveBlockUp(int line_number) {
    auto block = GetBlockAt(line_number);
    if (!block || block->start_line == 0) return false;
    
    // Find previous block
    std::shared_ptr<Block> prev_block = nullptr;
    for (auto& b : blocks_) {
        if (b->end_line < block->start_line) {
            if (!prev_block || b->end_line > prev_block->end_line) {
                prev_block = b;
            }
        }
    }
    
    if (!prev_block) return false;
    
    // 連続ブロックの順序を回転して入れ替え
    auto begin = lines_.begin() + prev_block->start_line;
    auto middle = lines_.begin() + block->start_line;
    auto end = lines_.begin() + block->end_line + 1;
    std::rotate(begin, middle, end);

    ParseBlocks(); // 再解析
    return true;
}

bool BlockModel::MoveBlockDown(int line_number) {
    auto block = GetBlockAt(line_number);
    if (!block || block->end_line >= static_cast<int>(lines_.size()) - 1) return false;
    
    // Find next block
    std::shared_ptr<Block> next_block = nullptr;
    for (auto& b : blocks_) {
        if (b->start_line > block->end_line) {
            if (!next_block || b->start_line < next_block->start_line) {
                next_block = b;
            }
        }
    }
    
    if (!next_block) return false;
    
    // 連続ブロックの順序を回転して入れ替え
    auto begin = lines_.begin() + block->start_line;
    auto middle = lines_.begin() + next_block->start_line;
    auto end = lines_.begin() + next_block->end_line + 1;
    std::rotate(begin, middle, end);

    ParseBlocks(); // 再解析
    return true;
}

std::shared_ptr<Block> BlockModel::GetBlockAt(int line_number) const {
    for (auto& block : blocks_) {
        if (line_number >= block->start_line && line_number <= block->end_line) {
            return block;
        }
    }
    return nullptr;
}

std::vector<std::string> BlockModel::GetVisibleLines() const {
    std::vector<std::string> out_lines;
    std::vector<int> out_indices;
    BuildVisibleView(out_lines, out_indices);
    return out_lines;
}

void BlockModel::UpdateLines() {
    ParseBlocks();
}

bool BlockModel::IsHeaderLine(const std::string& line, int& level) const {
    std::regex header_regex(R"(^(#{1,6})\s+(.*))");
    std::smatch matches;
    
    if (std::regex_match(line, matches, header_regex)) {
        level = matches[1].str().length();
        return true;
    }
    
    return false;
}

bool BlockModel::IsCodeFenceStart(const std::string& line) const {
    return line.find("```") == 0 || line.find("~~~") == 0;
}

bool BlockModel::IsQuoteLine(const std::string& line) const {
    return !line.empty() && line[0] == '>';
}

std::string BlockModel::ExtractHeaderText(const std::string& line) const {
    std::regex header_regex(R"(^#{1,6}\s+(.*))");
    std::smatch matches;
    
    if (std::regex_match(line, matches, header_regex)) {
        return matches[1].str();
    }
    
    return line;
}

std::vector<int> BlockModel::GetVisibleLineIndices() const {
    std::vector<std::string> out_lines;
    std::vector<int> out_indices;
    BuildVisibleView(out_lines, out_indices);
    return out_indices;
}

void BlockModel::BuildVisibleView(std::vector<std::string>& out_lines,
                                  std::vector<int>& out_indices) const {
    out_lines.clear();
    out_indices.clear();

    for (int i = 0; i < static_cast<int>(lines_.size()); ++i) {
        auto block = GetBlockAt(i);

        if (block && block->is_folded) {
            if (i == block->start_line) {
                out_lines.push_back(block->header_text + " [...]");
                out_indices.push_back(i);
            }
            continue;
        }
        out_lines.push_back(lines_[i]);
        out_indices.push_back(i);
    }
}

}
