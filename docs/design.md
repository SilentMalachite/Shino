# ShinoEditor Design Documentation

This document describes the high-level design and architecture of the ShinoEditor project.

## Architecture Overview

### Core Components
```
     +----------------+
     |      App      |
     +----------------+
            |
    +-------+--------+
    |       |        |
+--------+  |  +-----------+
|  UI/   |  |  | Document  |
|  TUI   |  |  | Handling |
+--------+  |  +-----------+
            |
    +-------+--------+
    |       |        |
+--------+  |  +-----------+
| Input  |  |  | File I/O  |
+--------+  |  +-----------+
```

### Component Roles
1. App: Main application coordination
2. UI/TUI: User interface (FTXUI-based)
3. Document Handling: Markdown processing
4. Input: Event handling and key bindings
5. File I/O: File operations and format conversion

## Design Principles

### 1. Component Separation
- Clear boundaries between components
- Minimal dependencies between modules
- Interface-based communication

### 2. Error Handling
- Structured error categories
- Detailed error information
- Recovery procedures
- Clear error messages

### 3. Security
- Path validation
- Command sanitization
- Memory safety
- Input validation

### 4. Performance
- Memory efficiency
- Caching strategies
- Large file handling
- Resource management

## Data Flow

### Document Processing
```
File -> App -> BlockModel -> MarkdownRenderer -> Display
  ^                   |
  |                   v
  +--- PandocIO <-- Export/Import
```

### User Input Flow
```
KeyPress -> TUIBindings -> App -> Action
                           |
                           v
                         State Update
                           |
                           v
                         UI Refresh
```

## Component Details

### 1. App Component
```cpp
class App {
    // State
    std::string filename_;
    std::vector<std::string> lines_;
    std::unique_ptr<BlockModel> block_model_;
    std::unique_ptr<MarkdownRenderer> renderer_;
    
    // UI components
    ftxui::Component main_component_;
    ftxui::Component editor_component_;
    ftxui::Component preview_component_;
    
    // UI state
    bool show_preview_;
    bool show_help_;
    int current_line_;
    // ...
};
```

#### Responsibilities
- Application lifecycle management
- Component coordination
- State management
- Event handling
- UI composition

### 2. BlockModel Component
```cpp
class BlockModel {
    std::vector<std::string>& lines_;
    std::vector<std::shared_ptr<Block>> blocks_;
    
    // Cache
    mutable std::vector<std::string> visible_lines_cache_;
    mutable std::vector<int> visible_indices_cache_;
    mutable bool cache_valid_;
};
```

#### Responsibilities
- Block detection
- Fold management
- Block movement
- View calculation
- Performance optimization

### 3. MarkdownRenderer Component
```cpp
class MarkdownRenderer {
    // Rendering options
    bool use_md4c_;
    RenderOptions options_;
    
    // Methods
    std::string RenderToText(const std::string& markdown);
    std::string RenderToHtml(const std::string& markdown);
};
```

#### Responsibilities
- Markdown parsing
- Text rendering
- HTML conversion (with md4c)
- UTF-8 handling

### 4. PandocIO Component
```cpp
class PandocIO {
    static std::string GenerateTempFileName();
    static std::string ExecutePandocCommand(const std::string& command);
    static std::string ShellEscape(const std::string& arg);
};
```

#### Responsibilities
- DOCX import/export
- Pandoc integration
- Temporary file management
- Command execution safety

## Key Algorithms

### 1. Block Detection
1. Iterate through lines
2. Pattern matching for block types
3. Block boundary detection
4. Metadata extraction
5. Cache management

### 2. Folding Logic
1. Block type identification
2. Fold state tracking
3. View recalculation
4. Cache invalidation
5. Visual state update

### 3. Block Movement
1. Block boundary detection
2. Line range rotation
3. Block list update
4. View recalculation
5. State preservation

### 4. Search Implementation
1. Query preparation
2. Text matching
3. Match list maintenance
4. Navigation management
5. UI feedback

## Performance Considerations

### Memory Management
1. Line buffer sharing
2. Smart pointer usage
3. Cache strategies
4. Vector pre-allocation
5. String view usage

### Large File Handling
1. Lazy loading
2. View windowing
3. Incremental updates
4. Memory monitoring
5. Cache size limits

## Security Measures

### File Operations
1. Path validation
2. Permission checks
3. Safe temporary files
4. Resource cleanup
5. Error isolation

### External Commands
1. Argument sanitization
2. Command validation
3. Process isolation
4. Resource limits
5. Error containment

## Testing Strategy

### Unit Tests
1. Component isolation
2. Edge case coverage
3. Error handling
4. Memory safety
5. Performance benchmarks

### Integration Tests
1. Component interaction
2. File operations
3. UI workflow
4. Error propagation
5. Resource management

## Future Considerations

### Potential Enhancements
1. Plugin architecture
2. Additional file formats
3. Extended search features
4. Collaboration features
5. Configuration system

### Performance Improvements
1. Incremental parsing
2. Parallel processing
3. Memory optimization
4. Cache strategies
5. I/O optimization

### Security Enhancements
1. File encryption
2. Secure IPC
3. Sandboxing
4. Access control
5. Audit logging