cmake_minimum_required(VERSION 3.28)
project(untitled1 C)

set(CMAKE_C_STANDARD 11)

# Define the paths for raylib and json-c
set(RAYLIB_DIR "~/Downloads/raylib-5.0_linux_amd64")
set(JSON_C_DIR "~/usr/include/json-c")  # Update with the actual path where json-c is installed

# Include directories for raylib and json-c
include_directories(${RAYLIB_DIR}/include)
include_directories(${JSON_C_DIR}/include)

# Link directories for raylib and json-c
link_directories(${RAYLIB_DIR}/lib)
link_directories(${JSON_C_DIR}/lib)

# Add the executable
add_executable(untitled1 cliente/main.c cliente/constants.h)

# Link the necessary libraries
target_link_libraries(untitled1 raylib m pthread dl json-c)
