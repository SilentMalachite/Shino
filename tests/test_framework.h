#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <filesystem>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <fstream>
#ifdef _WIN32
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#endif

// Test framework utilities
static int g_failures = 0;
static std::vector<std::string> g_test_names;
static std::vector<std::function<void()>> g_test_functions;

#define ASSERT_TRUE(cond) do { \
    if(!(cond)) { \
        std::cerr << __FILE__ << ":" << __LINE__ << " ASSERT_TRUE failed: " #cond "\n"; \
        ++g_failures; \
    } \
} while(0)

#define ASSERT_FALSE(cond) ASSERT_TRUE(!(cond))

#define ASSERT_EQ(a,b) do { \
    auto _va = (a); auto _vb = (b); \
    if(!((_va) == (_vb))) { \
        std::cerr << __FILE__ << ":" << __LINE__ << " ASSERT_EQ failed: " << _va << " != " << _vb << "\n"; \
        ++g_failures; \
    } \
} while(0)

#define ASSERT_NE(a,b) do { \
    auto _va = (a); auto _vb = (b); \
    if((_va) == (_vb)) { \
        std::cerr << __FILE__ << ":" << __LINE__ << " ASSERT_NE failed: " << _va << " == " << _vb << "\n"; \
        ++g_failures; \
    } \
} while(0)

// Test registration
#define TEST(test_name) \
    void test_##test_name(); \
    namespace { \
        struct test_##test_name##_register { \
            test_##test_name##_register() { \
                g_test_names.push_back(#test_name); \
                g_test_functions.push_back(test_##test_name); \
            } \
        } test_##test_name##_register_instance; \
    } \
    void test_##test_name()

// Common test utilities
namespace test_utils {
    // Create a temporary directory for test files
    inline std::filesystem::path create_temp_dir(const std::string& prefix) {
#ifdef _WIN32
        // Windows implementation
        auto temp_dir = std::filesystem::temp_directory_path() / (prefix + "_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
        std::filesystem::create_directories(temp_dir);
        return temp_dir;
#else
        auto temp_dir = std::filesystem::temp_directory_path() / (prefix + "_XXXXXX");
        char* dir_template = strdup(temp_dir.string().c_str());
        mkdtemp(dir_template);
        auto result = std::filesystem::path(dir_template);
        free(dir_template);
        return result;
#endif
    }

    // Create a temporary file with content
    inline std::filesystem::path create_temp_file(const std::string& content) {
#ifdef _WIN32
        // Windows implementation
        auto temp_path = std::filesystem::temp_directory_path() / ("test_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".tmp");
        std::ofstream file(temp_path);
        file << content;
        file.close();
        return temp_path;
#else
        auto temp_path = std::filesystem::temp_directory_path() / "test_XXXXXX";
        char* path_template = strdup(temp_path.string().c_str());
        int fd = mkstemp(path_template);
        if (fd != -1) {
            write(fd, content.c_str(), content.size());
            close(fd);
        }
        auto result = std::filesystem::path(path_template);
        free(path_template);
        return result;
#endif
    }

    // Clean up a temporary directory and its contents
    inline void cleanup_temp_dir(const std::filesystem::path& dir) {
        std::filesystem::remove_all(dir);
    }
}

// Main test runner
inline int run_all_tests() {
    for (size_t i = 0; i < g_test_functions.size(); ++i) {
        std::cout << "Running test: " << g_test_names[i] << "...\n";
        g_test_functions[i]();
    }
    if (g_failures == 0) {
        std::cout << "All " << g_test_functions.size() << " tests passed\n";
        return EXIT_SUCCESS;
    }
    std::cerr << g_failures << " test(s) failed\n";
    return EXIT_FAILURE;
}