cmake_minimum_required(VERSION 3.16)
project(Graphite LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# collect src files
file(GLOB_RECURSE GRAPHITE_SRC
    CONFIGURE_DEPENDS
    graphite/*.cpp
    utils/*.cpp
)

# imgui src files
set(IMGUI_SRC
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/backends/imgui_impl_dx11.cpp
    external/imgui/backends/imgui_impl_win32.cpp
)

# create executable
add_executable(Graphite WIN32
    graphite/main.cpp
    ${GRAPHITE_SRC}
    ${IMGUI_SRC}
)

# global definitions
target_compile_definitions(Graphite PRIVATE
    UNICODE
    _UNICODE
)

# include directories
target_include_directories(Graphite PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/graphite
    ${CMAKE_SOURCE_DIR}/utils
    ${CMAKE_SOURCE_DIR}/shaders
    ${CMAKE_SOURCE_DIR}/external/imgui
    ${CMAKE_SOURCE_DIR}/external/imgui/backends
    ${CMAKE_SOURCE_DIR}/external/entt/single_include/entt
    ${CMAKE_SOURCE_DIR}/external/glm
    ${CMAKE_SOURCE_DIR}/external/stb
    ${CMAKE_SOURCE_DIR}/external/spdlog/include
)

add_subdirectory(external/spdlog)

# link libraries
target_link_libraries(Graphite PRIVATE
    d3d11
    dxgi
    d3dcompiler
    spdlog
)

# set output directory
set_target_properties(Graphite PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# copy stuff to output directory
add_custom_command(TARGET Graphite POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
      "${CMAKE_SOURCE_DIR}/shaders"
      "$<TARGET_FILE_DIR:Graphite>/shaders"
  COMMAND ${CMAKE_COMMAND} -E copy_directory
      "${CMAKE_SOURCE_DIR}/assets"
      "$<TARGET_FILE_DIR:Graphite>/assets"
)