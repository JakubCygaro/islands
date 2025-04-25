cmake_minimum_required(VERSION 3.30)
project(islands VERSION 0.1.0.0)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    if(CMAKE_BUILD_TYPE STREQUAL "")
        set(CMAKE_BUILD_TYPE "Debug")
    endif()
endif()
find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(islands
    src/main.cc
    src/glad/glad.c
    src/shader/Shader.cc
    src/Camera.cc
    src/Game.cc
    src/Object.cc
)

target_include_directories(islands
    PRIVATE include/
)

target_link_libraries(islands
    PRIVATE glfw3
)

target_link_libraries(islands
    PRIVATE glm::glm
)

add_custom_target(copyGameData ALL)
add_custom_command(TARGET copyGameData
    POST_BUILD

    COMMAND ${CMAKE_COMMAND}
    ARGS -E echo "Copying game_data"

    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy_directory_if_different ${CMAKE_SOURCE_DIR}/game_data $<TARGET_FILE_DIR:${PROJECT_NAME}>/game_data
)
