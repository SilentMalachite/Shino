# ShinoEditor Development Guide

This document provides detailed information for developers working on ShinoEditor.

## Project Structure

```
ShinoEditor/
├── src/                    # Source code
│   ├── main.cpp           # Entry point
│   ├── app.*             # Application core
│   ├── block_model.*     # Block detection/movement
│   ├── markdown_renderer.* # Markdown rendering
│   ├── pandoc_io.*       # DOCX import/export
│   └── tui_bindings.*    # Key bindings/help
├── tests/                 # Test files
│   ├── test_framework.h   # Common test utilities
│   ├── app_test.*        # App tests
│   ├── block_model_test.* # Block model tests
│   ├── markdown_renderer_test.* # Renderer tests
│   └── pandoc_io_test.*  # Pandoc I/O tests
├── docs/                  # Documentation
│   ├── README.md         # Documentation index
│   └── development.md    # This file
├── CMakeLists.txt        # Build configuration
├── README.md             # User guide
├── CONTRIBUTING.md       # Contribution guidelines
└── CHANGELOG.md          # Version history
```

## Build System

The project uses CMake (3.16+) for build configuration. Key build options:

- `SHINO_BUILD_APP`: Build the main application (ON by default)
- `SHINO_BUILD_TESTS`: Build test executables (ON by default)
- `CMAKE_BUILD_TYPE`: Build type (Release/Debug)

Dependencies are managed as follows:
- FTXUI: Downloaded automatically if not found
- md4c: Optional, used for HTML rendering
- pandoc: Required for DOCX import/export

## Testing

Tests are organized by component:
- `block_model_tests`: Block detection and manipulation
- `markdown_renderer_tests`: Markdown rendering
- `pandoc_io_tests`: DOCX import/export
- `app_tests`: UI and integration tests

Run tests with:
```bash
cmake -S . -B build -DSHINO_BUILD_TESTS=ON
cmake --build build -j
ctest --test-dir build --output-on-failure
```

## Key Components

### App (app.h/cpp)
Central application class managing:
- UI state and components
- File operations
- User input handling
- Component coordination

### BlockModel (block_model.h/cpp)
Handles Markdown block operations:
- Block detection
- Folding/unfolding
- Block movement
- Line mapping

### MarkdownRenderer (markdown_renderer.h/cpp)
Provides Markdown rendering:
- Text rendering
- HTML rendering (via md4c)
- UTF-8 support

### PandocIO (pandoc_io.h/cpp)
Manages DOCX conversion:
- Import via pandoc
- Export via pandoc
- Error handling

### TUIBindings (tui_bindings.h/cpp)
Defines key bindings and help:
- Key constants
- Help text generation
- Key binding documentation