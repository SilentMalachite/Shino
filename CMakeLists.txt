cmake_minimum_required(VERSION 3.16)
project(ShinoEditor VERSION 1.1.0 LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)

# Try to find FTXUI
find_package(ftxui QUIET)
if(NOT ftxui_FOUND)
  # If not found, use FetchContent to download it
  include(FetchContent)
  FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
    GIT_TAG v5.0.0
  )
  FetchContent_MakeAvailable(ftxui)
endif()

# Try to find md4c (optional)
pkg_check_modules(MD4C md4c-html)
if(MD4C_FOUND)
    message(STATUS "Found md4c-html")
    add_compile_definitions(HAVE_MD4C)
else()
    message(STATUS "md4c-html not found, using fallback renderer")
endif()

# Create executable
add_executable(ShinoEditor
  src/main.cpp
  src/app.cpp
  src/block_model.cpp
  src/markdown_renderer.cpp
  src/pandoc_io.cpp
  src/tui_bindings.cpp
)

# Link libraries
target_link_libraries(ShinoEditor
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)

# Add md4c if available
if(MD4C_FOUND)
  target_link_libraries(ShinoEditor PRIVATE ${MD4C_LIBRARIES})
  target_include_directories(ShinoEditor PRIVATE ${MD4C_INCLUDE_DIRS})
  target_compile_options(ShinoEditor PRIVATE ${MD4C_CFLAGS_OTHER})
endif()

# Install target
install(TARGETS ShinoEditor DESTINATION bin)

# -----------------------
# Tests (header-only style)
# -----------------------
option(SHINO_BUILD_TESTS "Build Shino tests" ON)
if(SHINO_BUILD_TESTS)
  enable_testing()
  add_executable(sino_tests
    tests/block_model_test.cpp
    src/block_model.cpp
  )
  target_include_directories(sino_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
  target_compile_features(sino_tests PRIVATE cxx_std_20)
  add_test(NAME sino_block_model_tests COMMAND sino_tests)
endif()
