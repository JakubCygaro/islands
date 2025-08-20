cmake_minimum_required(VERSION 3.30)
project(islands VERSION 0.2.0.0)

set(FREETYPE_ROOT_PATHS "" CACHE FILEPATH "Where to search for the freetype library install directory")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    if(CMAKE_BUILD_TYPE STREQUAL "")
        set(CMAKE_BUILD_TYPE "Debug")
    endif()
endif()
find_package(glfw3 3.4 REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Freetype REQUIRED
    PATHS ${FREETYPE_ROOT_PATHS}
)

set(SCRIPTS_DIR ${CMAKE_SOURCE_DIR}/scripts)

file(GLOB_RECURSE GAME_DATA LIST_DIRECTORIES YES ${CMAKE_SOURCE_DIR}/game_data/*)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/files.hpp
    COMMAND python ${SCRIPTS_DIR}/gen_paths.py game_data/ ${CMAKE_BINARY_DIR}/files.hpp #--deps files.d
    DEPENDS ${GAME_DATA}
)

add_custom_target(gen_paths
    DEPENDS ${CMAKE_BINARY_DIR}/files.hpp
)

set(SHADERS_DIR ${CMAKE_SOURCE_DIR}/src/shaders)
set(SHADERS
    ${SHADERS_DIR}/c_body.frag
    ${SHADERS_DIR}/c_body.vert
    ${SHADERS_DIR}/font_bitmap.frag
    ${SHADERS_DIR}/font_bitmap.vert
    ${SHADERS_DIR}/planet.frag
    ${SHADERS_DIR}/planet.vert
    ${SHADERS_DIR}/star.frag
    ${SHADERS_DIR}/star.vert
    ${SHADERS_DIR}/test.frag
    ${SHADERS_DIR}/test.vert
    ${SHADERS_DIR}/text.frag
    ${SHADERS_DIR}/text.vert
    ${SHADERS_DIR}/normals.vert
    ${SHADERS_DIR}/normals.geom
    ${SHADERS_DIR}/normals.frag
)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/shaders.hpp
    COMMAND python ${SCRIPTS_DIR}/pack_shaders.py ${CMAKE_SOURCE_DIR}/src/shaders ${CMAKE_BINARY_DIR}/shaders.hpp
    DEPENDS ${SHADERS}
)
add_custom_target(pack_shaders
    DEPENDS ${CMAKE_BINARY_DIR}/shaders.hpp
)
add_custom_command(TARGET pack_shaders
    POST_BUILD

    COMMAND cmake
    ARGS -E echo "Shaders packed")

# this is for Debug build purposes, so i dont have to rebuild the whole project when woring with shaders
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/shader_files.hpp
    COMMAND python ${SCRIPTS_DIR}/gen_paths.py src/shaders ${CMAKE_BINARY_DIR}/shader_files.hpp
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS ${SHADERS}
)

add_custom_target(gen_shader_paths 
    DEPENDS ${CMAKE_BINARY_DIR}/shader_files.hpp
)

add_executable(islands
    src/main.cc
    src/glad/glad.c
    src/shader/Shader.cc
    src/Camera.cc
    src/Game.cc
    src/Game_ctors.cc
    src/Object.cc
    src/Object_ctors.cc
    src/Planet.cc
    src/Planet_ctors.cc
    src/Star.cc
    src/Star_ctors.cc
    src/Gui.cc
    src/KeyBind.cc
    src/Font.cc
    src/Font_ctors.cc
    src/imgui/imgui.cpp
    src/imgui/imgui_demo.cpp
    src/imgui/imgui_draw.cpp
    src/imgui/imgui_tables.cpp
    src/imgui/imgui_widgets.cpp
    src/imgui/imgui_stdlib.cpp
    src/imgui/backend/imgui_impl_glfw.cpp
    src/imgui/backend/imgui_impl_opengl3.cpp
    ${CMAKE_BINARY_DIR}/files.hpp
    ${CMAKE_BINARY_DIR}/shaders.hpp
    $<$<CONFIG:Debug>:${CMAKE_BINARY_DIR}/shader_files.hpp>
)

configure_file(global_declarations.hpp.in global_declarations.hpp @ONLY)

add_library(features INTERFACE)

target_compile_features(features INTERFACE cxx_std_20)

target_compile_options(islands
    PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    PRIVATE $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>
)
target_compile_definitions(islands
    PUBLIC $<$<CONFIG:Debug>:DEBUG>
    PUBLIC $<$<CONFIG:Release>:RELEASE>
)
target_link_libraries(islands PRIVATE features)

target_include_directories(islands
    PUBLIC include/
    PUBLIC ${CMAKE_BINARY_DIR}
)

target_link_libraries(islands
    PRIVATE glfw
    PRIVATE glm::glm
    PRIVATE Freetype::Freetype
)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_target(copyGameData ALL)
    add_custom_command(TARGET copyGameData
        POST_BUILD

        COMMAND ${CMAKE_COMMAND}
        ARGS -E echo "Copying game_data"

        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy_directory_if_different ${CMAKE_SOURCE_DIR}/game_data $<TARGET_FILE_DIR:${PROJECT_NAME}>/game_data
    )
endif()
