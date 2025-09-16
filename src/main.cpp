#include "app.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    try {
        ShinoEditor::App app;
        
        std::string filename;
        if (argc > 1) {
            filename = argv[1];
        }
        
        return app.Run(filename);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}
