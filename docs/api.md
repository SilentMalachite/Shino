# ShinoEditor API Documentation

This document provides detailed information about the public APIs of ShinoEditor's core components.

## Table of Contents
1. [App Class](#app-class)
2. [BlockModel Class](#blockmodel-class)
3. [MarkdownRenderer Class](#markdownrenderer-class)
4. [PandocIO Class](#pandocio-class)
5. [Error Handling](#error-handling)
6. [Security Utilities](#security-utilities)

## App Class

### Overview
`App` is the main application class that coordinates all components and handles user interaction.

### Public Methods
```cpp
class App {
public:
    App();
    ~App();
    
    // Run the application
    // @param filename Optional filename to open at startup
    // @return Exit code (0 for success)
    int Run(const std::string& filename = "");
};
```

### State Management
The App class maintains application state including:
- Current file and content
- UI state (preview, help, search)
- Editor state (cursor, folding)
- Modal dialogs (filename prompt, search)

### Event Handling
Handles keyboard events for:
- File operations (Ctrl+O, Ctrl+X)
- Editor functions (Ctrl+W, Ctrl+J)
- View toggles (Ctrl+P, Ctrl+G)
- Text input and editing

## BlockModel Class

### Overview
`BlockModel` manages Markdown block detection, folding, and movement operations.

### Public Methods
```cpp
class BlockModel {
public:
    explicit BlockModel(std::vector<std::string>& lines);
    
    // Block operations
    void ParseBlocks();
    void ToggleFold(int line_number);
    bool MoveBlockUp(int line_number);
    bool MoveBlockDown(int line_number);
    
    // Block queries
    std::shared_ptr<Block> GetBlockAt(int line_number) const;
    const std::vector<std::shared_ptr<Block>>& GetBlocks() const;
    
    // View operations
    std::vector<std::string> GetVisibleLines() const;
    std::vector<int> GetVisibleLineIndices() const;
    void UpdateLines();
};
```

### Block Types
```cpp
enum class BlockType {
    PARAGRAPH,
    HEADER,
    CODE_FENCE,
    QUOTE
};

struct Block {
    BlockType type;
    int start_line;
    int end_line;
    int level;          // For headers (1-6) or quote depth
    bool is_folded;
    std::string header_text;
};
```

### Performance Considerations
- Uses caching for visible lines
- Pre-allocates vectors
- Optimizes regex patterns
- Minimizes memory allocations

## MarkdownRenderer Class

### Overview
`MarkdownRenderer` provides text and HTML rendering for Markdown content.

### Public Methods
```cpp
class MarkdownRenderer {
public:
    MarkdownRenderer();
    
    // Convert Markdown to plain text
    std::string RenderToText(const std::string& markdown);
    
    // Convert Markdown to HTML (requires md4c)
    std::string RenderToHtml(const std::string& markdown);
};
```

### Supported Features
- Headers (1-6 levels)
- Lists (ordered and unordered)
- Code blocks
- Block quotes
- Inline formatting
- UTF-8 text

## PandocIO Class

### Overview
`PandocIO` handles DOCX file import/export via pandoc.

### Public Methods
```cpp
class PandocIO {
public:
    // Check pandoc availability
    static bool IsPandocAvailable();
    
    // Import DOCX to Markdown
    static std::optional<std::string> ImportDocx(const std::string& docx_path);
    
    // Export Markdown to DOCX
    static bool ExportDocx(const std::string& markdown_content,
                          const std::string& docx_path);
    
    // Get pandoc version info
    static std::string GetPandocVersion();
};
```

### Security Features
- Path validation
- Shell command escaping
- Error handling
- Temporary file management

## Error Handling

### Overview
ShinoEditor uses a structured error handling system based on `ShinoError`.

### Error Categories
```cpp
enum class Category {
    File,      // File operations
    Parser,    // Markdown parsing
    Convert,   // Format conversion
    UI,        // User interface
    System     // System/OS errors
};
```

### Error Helpers
```cpp
namespace error {
    // File operations
    void ThrowFileNotFound(const std::string& path);
    void ThrowFileNotWritable(const std::string& path);
    
    // Conversion
    void ThrowConversionFailed(const std::string& from,
                              const std::string& to,
                              const std::string& detail);
    
    // System
    void ThrowSystemError(const std::string& operation,
                         const std::string& detail);
}
```

## Security Utilities

### Path Validation
```cpp
class PathValidator {
public:
    // Validate path for security issues
    static void ValidatePathSecurity(const std::string& path);
    
    // Validate file operation with permissions
    static void ValidateFileOperation(const std::string& path,
                                    bool write = false);
};
```

### Command Validation
```cpp
class CommandValidator {
public:
    // Validate shell command
    static void ValidateCommand(const std::string& cmd);
    
    // Safe shell argument escaping
    static std::string SafeShellEscape(const std::string& arg);
    
    // Build safe shell command
    static std::string BuildSafeCommand(const std::string& program,
                                      const std::vector<std::string>& args);
};
```