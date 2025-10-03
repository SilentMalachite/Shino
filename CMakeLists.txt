cmake_minimum_required(VERSION 3.16)
project(ShinoEditor VERSION 1.2.2 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# macOS deployment target for std::filesystem support
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum macOS deployment version")
endif()

# Set build type if not specified (default to Release for performance)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

option(SHINO_BUILD_APP "Build ShinoEditor application" ON)
option(SHINO_BUILD_TESTS "Build Shino tests" ON)

# Find optional packages
find_package(PkgConfig QUIET)

# FTXUI is needed for both app and tests
if(SHINO_BUILD_APP OR SHINO_BUILD_TESTS)
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
endif()

if(SHINO_BUILD_APP)
  # Try to find md4c (optional)
  if(PkgConfig_FOUND)
    pkg_check_modules(MD4C md4c-html)
  endif()
  if(MD4C_FOUND)
    message(STATUS "Found md4c-html")
    add_compile_definitions(HAVE_MD4C)
  else()
    message(STATUS "md4c-html not found, using fallback renderer")
  endif()
endif()

if(SHINO_BUILD_APP)
  # Create executable
  add_executable(ShinoEditor
    src/main.cpp
    src/app.cpp
    src/block_model.cpp
    src/markdown_renderer.cpp
    src/pandoc_io.cpp
    src/tui_bindings.cpp
  )

  # Set C++ standard for target
  target_compile_features(ShinoEditor PRIVATE cxx_std_20)

  # Link libraries
  target_link_libraries(ShinoEditor
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
endif()

# -----------------------
# Tests (header-only style)
# -----------------------
option(SHINO_BUILD_PERF_TESTS "Build performance tests" ON)

if(SHINO_BUILD_TESTS)
  enable_testing()
  
  # Original unit test
  add_executable(sino_tests
    tests/block_model_test.cpp
    src/block_model.cpp
  )
  target_include_directories(sino_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
  target_compile_features(sino_tests PRIVATE cxx_std_20)
  add_test(NAME sino_block_model_tests COMMAND sino_tests)
  
  # Add new unit tests
  add_executable(markdown_renderer_tests
    tests/markdown_renderer_test.cpp
    src/markdown_renderer.cpp
  )
  target_include_directories(markdown_renderer_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
  target_compile_features(markdown_renderer_tests PRIVATE cxx_std_20)
  if(MD4C_FOUND)
    target_link_libraries(markdown_renderer_tests PRIVATE ${MD4C_LIBRARIES})
    target_include_directories(markdown_renderer_tests PRIVATE ${MD4C_INCLUDE_DIRS})
    target_compile_options(markdown_renderer_tests PRIVATE ${MD4C_CFLAGS_OTHER})
  endif()
  add_test(NAME markdown_renderer_tests COMMAND markdown_renderer_tests)
  
  add_executable(pandoc_io_tests
    tests/pandoc_io_test.cpp
    src/pandoc_io.cpp
  )
  target_include_directories(pandoc_io_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
  target_compile_features(pandoc_io_tests PRIVATE cxx_std_20)
  add_test(NAME pandoc_io_tests COMMAND pandoc_io_tests)
  
  add_executable(app_tests
    tests/app_test.cpp
    src/app.cpp
    src/block_model.cpp
    src/markdown_renderer.cpp
    src/pandoc_io.cpp
    src/tui_bindings.cpp
  )
  target_include_directories(app_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
  target_compile_features(app_tests PRIVATE cxx_std_20)
  target_link_libraries(app_tests PRIVATE ftxui::component)
  if(MD4C_FOUND)
    target_link_libraries(app_tests PRIVATE ${MD4C_LIBRARIES})
    target_include_directories(app_tests PRIVATE ${MD4C_INCLUDE_DIRS})
    target_compile_options(app_tests PRIVATE ${MD4C_CFLAGS_OTHER})
  endif()
  add_test(NAME app_tests COMMAND app_tests)
endif()

if(SHINO_BUILD_PERF_TESTS)
  add_executable(perf_tests
    tests/perf_test.cpp
    src/block_model.cpp
    src/markdown_renderer.cpp
    src/pandoc_io.cpp
  )
  target_include_directories(perf_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
  target_compile_features(perf_tests PRIVATE cxx_std_20)
  if(MD4C_FOUND)
    target_link_libraries(perf_tests PRIVATE ${MD4C_LIBRARIES})
    target_include_directories(perf_tests PRIVATE ${MD4C_INCLUDE_DIRS})
    target_compile_options(perf_tests PRIVATE ${MD4C_CFLAGS_OTHER})
  endif()
  add_test(NAME performance_tests COMMAND perf_tests)
endif()
