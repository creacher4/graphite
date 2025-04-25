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
    graphite/rendering/Gbuffer.cpp
    graphite/resources/ResourceManager.cpp

    # utils
    utils/ShaderUtils.cpp


    # external sources
    external/imgui/imgui.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_dx11.cpp
    external/imgui/backends/imgui_impl_win32.cpp

)

# add include directories
target_include_directories(Graphite PRIVATE
    ${CMAKE_SOURCE_DIR}/graphite
    ${CMAKE_SOURCE_DIR}/utils
    ${CMAKE_SOURCE_DIR}/shaders
    ${CMAKE_SOURCE_DIR}/external/entt/single_include/entt
    ${CMAKE_SOURCE_DIR}/external/glm
    ${CMAKE_SOURCE_DIR}/external/imgui
    ${CMAKE_SOURCE_DIR}/external/imgui/backends
)

target_link_libraries(Graphite PRIVATE
    d3d11
    dxgi
    d3dcompiler
)

set_target_properties(Graphite PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# add_custom_command(TARGET Graphite POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E make_directory
#         "$<TARGET_FILE_DIR:Graphite>/shaders"
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#         "${CMAKE_SOURCE_DIR}/shaders"
#         "$<TARGET_FILE_DIR:Graphite>/shaders"
# )