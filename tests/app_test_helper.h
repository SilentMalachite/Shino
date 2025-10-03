#pragma once
#include "app.h"
#include "tui_bindings.h"
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

namespace ShinoEditor {
namespace test {

// Helper class to simulate user interaction with the App
class AppTestHelper {
public:
    AppTestHelper() {
        app_ = std::make_unique<App>();
    }

    // Helper to simulate keyboard input
    void SendKeys(const std::vector<std::string>& keys) {
        for (const auto& key : keys) {
            app_->HandleKeyPress(ftxui::Event::Character(key));
        }
    }

    // Helper to simulate special keys
    void SendSpecialKey(ftxui::Event event) {
        app_->HandleKeyPress(event);
    }

    // Helper to send control keys (e.g., Ctrl+O = char 15)
    void SendControlKey(int code) {
        app_->HandleKeyPress(ftxui::Event::Character(std::string(1, static_cast<char>(code))));
    }

    // Run the app in a separate thread with timeout
    int RunWithTimeout(const std::string& filename = "", int timeout_ms = 1000) {
        int result = 0;
        std::thread app_thread([&] {
            result = app_->Run(filename);
        });

        // Wait for timeout
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));

        // Force exit
        SendControlKey(TUIBindings::CTRL_X);
        
        if (app_thread.joinable()) {
            app_thread.join();
        }
        
        return result;
    }

    // Get the app instance for direct state checks
    App* GetApp() { return app_.get(); }

private:
    std::unique_ptr<App> app_;
};

} // namespace test
} // namespace ShinoEditor