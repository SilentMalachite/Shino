#pragma once
#include <string>
#include <vector>

namespace ShinoEditor {

struct KeyBinding {
    std::string key;
    std::string description;
};

class TUIBindings {
public:
    // Key constants
    static constexpr int CTRL_O = 15;  // Save
    static constexpr int CTRL_X = 24;  // Exit
    static constexpr int CTRL_W = 23;  // Search
    static constexpr int CTRL_G = 7;   // Help
    static constexpr int CTRL_J = 10;  // Fold/Unfold block
    static constexpr int CTRL_P = 16;  // Preview toggle
    static constexpr int CTRL_I = 9;   // Import DOCX
    static constexpr int CTRL_E = 5;   // Export DOCX
    
    // Get help line text
    static std::string GetHelpLine();
    
    // Get all key bindings
    static std::vector<KeyBinding> GetAllBindings();
};

}
