cmake_minimum_required(VERSION 3.20)
project(STL_Explorer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# FTXUI configuration
set(FTXUI_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(FTXUI_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(FTXUI_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)

# Fetch FTXUI
include(FetchContent)
FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
    GIT_TAG v5.0.0
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(ftxui)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Source files
set(SOURCES
    src/main.cpp
    src/core/renderer/renderer.cpp
    src/core/renderer/component_factory.cpp
    src/core/animation/animation.cpp
    src/core/animation/animation_engine.cpp
    src/core/animation/easing.cpp
    src/core/memory/memory_visualizer.cpp
    src/core/complexity/complexity_panel.cpp
    src/core/quiz/quiz_module.cpp
    src/core/compare/compare_module.cpp
    src/utils/theme.cpp
    src/utils/logger.cpp
    src/utils/utils.cpp
    src/containers/vector/vector_module.cpp
    src/containers/vector/vector_visualizer.cpp
    src/containers/stack/stack_module.cpp
    src/containers/stack/stack_visualizer.cpp
    src/containers/queue/queue_module.cpp
    src/containers/queue/queue_visualizer.cpp
    src/containers/deque/deque_module.cpp
    src/containers/deque/deque_visualizer.cpp
    src/containers/list/list_module.cpp
    src/containers/list/list_visualizer.cpp
    src/containers/map/map_module.cpp
    src/containers/map/map_visualizer.cpp
    src/containers/unordered_map/unordered_map_module.cpp
    src/containers/unordered_map/unordered_map_visualizer.cpp
    src/containers/set/set_module.cpp
    src/containers/set/set_visualizer.cpp
    src/containers/priority_queue/priority_queue_module.cpp
    src/containers/priority_queue/priority_queue_visualizer.cpp
    src/algorithms/sorting/sorting_algorithms.cpp
    src/algorithms/sorting/sorting_visualizer.cpp
    src/algorithms/searching/searching_algorithms.cpp
    src/algorithms/searching/searching_visualizer.cpp
)

# Executable
add_executable(stl_explorer ${SOURCES})

target_link_libraries(stl_explorer
    PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(stl_explorer PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wno-unused-parameter
    )
endif()

# Installation
install(TARGETS stl_explorer RUNTIME DESTINATION bin)

# Testing
enable_testing()
