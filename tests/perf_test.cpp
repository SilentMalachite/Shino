#include "perf_test_framework.h"
#include "block_model.h"
#include "markdown_renderer.h"
#include "pandoc_io.h"
#include <memory>
#include <vector>
#include <iostream>
#include <filesystem>
#include <fstream>

using namespace ShinoEditor;

void TestBlockModel() {
    std::cout << "\nTesting BlockModel Performance\n";
    std::cout << "============================\n";
    
    std::vector<perf::Benchmark::Result> results;
    
    // Test with various file sizes
    for (size_t size_kb : {100, 500, 1000}) {
        std::string content = perf::TestDataGenerator::GenerateLargeMarkdown(size_kb);
        std::vector<std::string> lines;
        
        // Split content into lines
        std::istringstream iss(content);
        std::string line;
        while (std::getline(iss, line)) {
            lines.push_back(line);
        }
        
        // Test block detection
        results.push_back(perf::Benchmark::Run(
            "Block Detection (" + std::to_string(size_kb) + "KB)",
            10,
            [&]() {
                auto model = std::make_unique<BlockModel>(lines);
                model->UpdateLines();
            }
        ));
        
        // Set up a model for fold operations
        auto model = std::make_unique<BlockModel>(lines);
        model->UpdateLines();
        
        // Test folding operations
        results.push_back(perf::Benchmark::Run(
            "Block Folding (" + std::to_string(size_kb) + "KB)",
            100,
            [&]() {
                for (int i = 0; i < static_cast<int>(lines.size()); i += 10) {
                    model->ToggleFold(i);
                }
            }
        ));
        
        // Test block movement
        results.push_back(perf::Benchmark::Run(
            "Block Movement (" + std::to_string(size_kb) + "KB)",
            100,
            [&]() {
                for (int i = 0; i < static_cast<int>(lines.size()); i += 20) {
                    model->MoveBlockDown(i);
                    model->MoveBlockUp(i + 10);
                }
            }
        ));
    }
    
    perf::Benchmark::Report(results);
}

void TestMarkdownRenderer() {
    std::cout << "\nTesting MarkdownRenderer Performance\n";
    std::cout << "=================================\n";
    
    std::vector<perf::Benchmark::Result> results;
    auto renderer = std::make_unique<MarkdownRenderer>();
    
    // Test with various file sizes
    for (size_t size_kb : {100, 500, 1000}) {
        std::string content = perf::TestDataGenerator::GenerateLargeMarkdown(size_kb);
        
        // Test text rendering
        results.push_back(perf::Benchmark::Run(
            "Text Rendering (" + std::to_string(size_kb) + "KB)",
            10,
            [&]() {
                renderer->RenderToText(content);
            }
        ));
        
        // Test HTML rendering
        results.push_back(perf::Benchmark::Run(
            "HTML Rendering (" + std::to_string(size_kb) + "KB)",
            10,
            [&]() {
                renderer->RenderToHtml(content);
            }
        ));
    }
    
    perf::Benchmark::Report(results);
}

void TestPandocIO() {
    if (!PandocIO::IsPandocAvailable()) {
        std::cout << "\nSkipping PandocIO Performance Tests (pandoc not available)\n";
        return;
    }
    
    std::cout << "\nTesting PandocIO Performance\n";
    std::cout << "==========================\n";
    
    std::vector<perf::Benchmark::Result> results;
    
    // Test with various file sizes
    for (size_t size_kb : {100, 500, 1000}) {
        std::string content = perf::TestDataGenerator::GenerateLargeMarkdown(size_kb);
        
        // Create a temporary file path
        namespace fs = std::filesystem;
        auto temp_dir = fs::temp_directory_path();
        auto temp_docx = temp_dir / ("test_" + std::to_string(size_kb) + ".docx");
        
        // Test DOCX export
        results.push_back(perf::Benchmark::Run(
            "DOCX Export (" + std::to_string(size_kb) + "KB)",
            5,
            [&]() {
                PandocIO::ExportDocx(content, temp_docx.string());
            }
        ));
        
        // Test DOCX import (if export succeeded)
        if (fs::exists(temp_docx)) {
            results.push_back(perf::Benchmark::Run(
                "DOCX Import (" + std::to_string(size_kb) + "KB)",
                5,
                [&]() {
                    PandocIO::ImportDocx(temp_docx.string());
                }
            ));
            
            // Clean up
            fs::remove(temp_docx);
        }
    }
    
    perf::Benchmark::Report(results);
}

int main() {
    std::cout << "Running Performance Tests\n";
    std::cout << "=======================\n";
    
    TestBlockModel();
    TestMarkdownRenderer();
    TestPandocIO();
    
    return 0;
}