# ShinoEditor Project Knowledge

## Project Overview
Terminal-based Markdown editor written in C++20 with FTXUI. Designed for novel and long document editing with nano-style key bindings.

## Key Features
- Block folding for headers, code fences, and quotes
- Block movement with Alt+↑/↓
- nano-style shortcuts (Ctrl+O save, Ctrl+X exit, etc.)
- DOCX import/export via pandoc
- Live HTML preview with Ctrl+P

## Build System
- Uses C++20 standard
- Dependencies: FTXUI, md4c
- External tool: pandoc (for DOCX support)

## Architecture
- UI: FTXUI ScreenInteractive + Component
- Text model: BlockModel for paragraph detection
- Rendering: markdown_renderer (md4c wrapper)
- I/O: pandoc_io for DOCX operations

## Development Notes
- Focus on learning curve minimization
- Maintain nano compatibility for familiar users
- Block-oriented editing paradigm
- Text editing: Enter edit mode with any character, Enter to save, Esc to cancel
- Block movement: Use Page Up/Down (Alt+Arrow keys challenging in terminals)
- DOCX support requires pandoc installation

## Implementation Status
✅ **Completed Features:**
- Nano-style key bindings (Ctrl+O, Ctrl+X, etc.)
- Block detection and folding (headers, code fences, quotes)
- Block movement with Page Up/Down
- Basic text editing (insert/delete lines, edit mode)
- File loading and saving
- DOCX import/export via pandoc
- HTML preview display
- Help system with Ctrl+G
- TUI interface with FTXUI

🔄 **Future Enhancements:**
- Search functionality (Ctrl+W)
- Better Alt+Arrow key detection
- Interactive file dialogs for DOCX operations
- More advanced text editing (character-level)
- Undo/redo functionality
