#pragma once
#include <chrono>
#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <random>

namespace ShinoEditor {
namespace perf {

// Performance measurement utilities
class Timer {
public:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;
    using Duration = std::chrono::microseconds;
    
    Timer() : start_(Clock::now()) {}
    
    void Reset() {
        start_ = Clock::now();
    }
    
    int64_t ElapsedMicros() const {
        auto now = Clock::now();
        return std::chrono::duration_cast<Duration>(now - start_).count();
    }
    
    double ElapsedMillis() const {
        return ElapsedMicros() / 1000.0;
    }
    
private:
    TimePoint start_;
};

// Test data generation
class TestDataGenerator {
public:
    static std::string GenerateLargeMarkdown(size_t target_size_kb) {
        const size_t target_bytes = target_size_kb * 1024;
        std::stringstream ss;
        
        // Header patterns for markdown blocks
        const std::vector<std::string> headers = {
            "# ", "## ", "### ", "#### ", "##### ", "###### "
        };
        
        // Generate content until we reach target size
        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<> header_dist(0, headers.size() - 1);
        std::uniform_int_distribution<> line_len_dist(20, 80);
        std::uniform_int_distribution<> block_size_dist(3, 10);
        
        while (ss.tellp() < static_cast<std::streampos>(target_bytes)) {
            // Add a header
            ss << headers[header_dist(rng)];
            ss << GenerateTitle() << "\n\n";
            
            // Add paragraph or list
            int block_size = block_size_dist(rng);
            if (rng() % 2 == 0) {
                // Paragraph
                for (int i = 0; i < block_size; i++) {
                    ss << GenerateParagraph(line_len_dist(rng)) << "\n";
                }
            } else {
                // List
                for (int i = 0; i < block_size; i++) {
                    ss << "- " << GenerateSentence(line_len_dist(rng)) << "\n";
                }
            }
            ss << "\n";
            
            // Sometimes add a code block
            if (rng() % 4 == 0) {
                ss << "```\n";
                for (int i = 0; i < block_size; i++) {
                    ss << GenerateCode(line_len_dist(rng)) << "\n";
                }
                ss << "```\n\n";
            }
        }
        
        return ss.str();
    }
    
private:
    static std::string GenerateTitle() {
        static const std::vector<std::string> words = {
            "Performance", "Testing", "Document", "Section", "Chapter",
            "Analysis", "Implementation", "Reference", "Overview", "Summary"
        };
        static std::mt19937 rng(std::random_device{}());
        static std::uniform_int_distribution<> word_dist(0, words.size() - 1);
        
        std::stringstream ss;
        ss << words[word_dist(rng)] << " " << words[word_dist(rng)];
        return ss.str();
    }
    
    static std::string GenerateSentence(int length) {
        static const std::vector<std::string> words = {
            "the", "be", "to", "of", "and", "a", "in", "that", "have", "I",
            "it", "for", "not", "on", "with", "he", "as", "you", "do", "at"
        };
        static std::mt19937 rng(std::random_device{}());
        static std::uniform_int_distribution<> word_dist(0, words.size() - 1);
        
        std::stringstream ss;
        int current_length = 0;
        while (current_length < length) {
            ss << words[word_dist(rng)] << " ";
            current_length += words[word_dist(rng)].length() + 1;
        }
        return ss.str();
    }
    
    static std::string GenerateParagraph(int length) {
        return GenerateSentence(length) + " " + GenerateSentence(length);
    }
    
    static std::string GenerateCode(int length) {
        static const std::vector<std::string> code_words = {
            "void", "int", "return", "if", "else", "for", "while",
            "class", "struct", "public", "private", "static", "const"
        };
        static std::mt19937 rng(std::random_device{}());
        static std::uniform_int_distribution<> word_dist(0, code_words.size() - 1);
        
        std::stringstream ss;
        int current_length = 0;
        while (current_length < length) {
            ss << code_words[word_dist(rng)] << " ";
            current_length += code_words[word_dist(rng)].length() + 1;
        }
        return ss.str();
    }
};

// Benchmarking utilities
class Benchmark {
public:
    struct Result {
        std::string name;
        int64_t duration_micros;
        size_t iterations;
        
        double AverageMillis() const {
            return (duration_micros / static_cast<double>(iterations)) / 1000.0;
        }
    };
    
    static Result Run(const std::string& name,
                     size_t iterations,
                     std::function<void()> fn) {
        Timer timer;
        for (size_t i = 0; i < iterations; i++) {
            fn();
        }
        return Result{name, timer.ElapsedMicros(), iterations};
    }
    
    static void Report(const std::vector<Result>& results) {
        std::cout << "\nBenchmark Results:\n";
        std::cout << "==================\n";
        
        for (const auto& result : results) {
            std::cout << result.name << ":\n";
            std::cout << "  Total time: " << (result.duration_micros / 1000.0) << " ms\n";
            std::cout << "  Iterations: " << result.iterations << "\n";
            std::cout << "  Average: " << result.AverageMillis() << " ms\n";
            std::cout << "\n";
        }
    }
};

} // namespace perf
} // namespace ShinoEditor