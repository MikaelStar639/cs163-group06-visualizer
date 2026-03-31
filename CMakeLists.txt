cmake_minimum_required(VERSION 3.15)
project(cs163-group06-visualizer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Apply static linking only for MinGW/GCC to avoid breaking other compilers
if(MINGW OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
endif()

# SFML Configuration - Using relative paths is perfect
set(SFML_DIR "${CMAKE_SOURCE_DIR}/external/SFML/lib/cmake/SFML")
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System Audio)

# Your project's own include folder
include_directories(include)

# Automatically find all your .cpp files
file(GLOB_RECURSE SOURCES src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
        SFML::Audio
)

# Assets Copying
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "${CMAKE_BINARY_DIR}/assets"
)
add_dependencies(${PROJECT_NAME} copy_assets)

# DLL Copying (Post-Build)
if(WIN32) # Only run this on Windows
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/external/SFML/bin"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )
endif()