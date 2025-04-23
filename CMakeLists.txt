cmake_minimum_required(VERSION 3.16)
project(Graphite LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# global definition
add_definitions(-DUNICODE -D_UNICODE)

# define executable
add_executable(Graphite WIN32
    
    # entry point
    graphite/main.cpp

    # source files
    graphite/core/Application.cpp
    graphite/core/Engine.cpp
    graphite/platform/PlatformWindow.cpp
    graphite/rendering/Renderer.cpp
)

# add include directories
target_include_directories(Graphite PRIVATE
    graphite
    external/entt/single_include/entt
)

target_link_libraries(Graphite PRIVATE
    d3d11
    dxgi
)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)