cmake_minimum_required(VERSION 3.10)

project(balash)
set(CMAKE_CXX_STANDARD 17)

# Set debug flags
set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")

# Collect all headers and source files
file(GLOB_RECURSE HEADERS 
    "include/*.hpp"
    "*.hpp"
    "loaders/*.hpp"
    "utils/*.hpp"
    "objects/**/*.hpp"
)

file(GLOB_RECURSE SOURCES
    "*.cpp"
    "loaders/*.cpp"
    "utils/*.cpp"
    "objects/**/*.cpp"
)

# Debug build
set(CMAKE_BUILD_TYPE Debug)

# Find required packages
find_package(fmt CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(PNG REQUIRED)
find_package(assimp REQUIRED)

# Add the executable
add_executable(balash ${SOURCES} ${HEADERS})

# Link libraries to the executable
target_link_libraries(balash PRIVATE fmt::fmt)
target_link_libraries(balash PRIVATE GLEW::GLEW)
target_link_libraries(balash PRIVATE glfw)
target_link_libraries(balash PRIVATE PNG::PNG)
target_link_libraries(balash PRIVATE assimp-vc143-mtd)