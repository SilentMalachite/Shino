#include "app.h"
#include "error_handler.h"
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
    } catch (const ShinoEditor::ShinoError& e) {
        // Format error based on category
        std::cerr << e.what() << std::endl;
        
        // Provide helpful recovery hints based on error category
        switch (e.category()) {
            case ShinoEditor::ShinoError::Category::File:
                std::cerr << "Hint: Check file permissions and path" << std::endl;
                break;
            case ShinoEditor::ShinoError::Category::Convert:
                std::cerr << "Hint: Ensure pandoc is installed and input format is correct" << std::endl;
                break;
            case ShinoEditor::ShinoError::Category::System:
                std::cerr << "Hint: Check system resources and permissions" << std::endl;
                break;
            default:
                break;
        }
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Critical error: Unknown exception occurred" << std::endl;
        return 1;
    }
}
