cmake_minimum_required(VERSION 3.10)

project(a3)
set(CMAKE_CXX_STANDARD 11)

# Ensure we're compiling for x86_64 on macOS
set(CMAKE_OSX_ARCHITECTURES "x86_64")
cmake_policy(SET CMP0072 NEW)

# Set compilers
set(CMAKE_C_COMPILER /usr/local/bin/gcc-14)
set(CMAKE_CXX_COMPILER /usr/local/bin/g++-14)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(OpenMP REQUIRED)

# Find and explicitly link SDL2 and SDL2_image (x86_64 versions)
find_library(SDL2_LIBRARY NAMES SDL2 PATHS /usr/local/lib)
find_library(SDL2_IMAGE_LIBRARY NAMES SDL2_image PATHS /usr/local/lib)

# Check if libraries were found
if(NOT SDL2_LIBRARY OR NOT SDL2_IMAGE_LIBRARY)
    message(FATAL_ERROR "SDL2 or SDL2_image not found in /usr/local/lib. Ensure you installed the x86_64 version via Rosetta.")
endif()

# Include directories
set(GLM_INCLUDE_DIR "/usr/local/Cellar/glm/1.0.1/include")
include_directories(${GLM_INCLUDE_DIR})
include_directories(revamp_src/classes revamp_src/image)

# Collect all source files
file(GLOB_RECURSE CLASS_SOURCES revamp_src/classes/*.cpp)
file(GLOB IMAGE_SOURCES revamp_src/image/*.cpp)

# Create static library
add_library(ray_tracer STATIC ${CLASS_SOURCES} ${IMAGE_SOURCES})
target_link_libraries(ray_tracer glm::glm OpenMP::OpenMP_CXX)

# Compiler options
add_compile_options(-O3 -Wall)

# Add executables
set(EXECUTABLES example e1 e2 e3 e4 e5 e6 e7 e8 e9 e10)

# Create all executables
foreach(EXECUTABLE ${EXECUTABLES})
    add_executable(${EXECUTABLE} executables/${EXECUTABLE}.cpp)
    
    # Link all necessary libraries to each executable
    target_link_libraries(${EXECUTABLE} ray_tracer ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} OpenGL::GL OpenMP::OpenMP_CXX)
    
    # Apply compile options to each executable
    target_compile_options(${EXECUTABLE} PRIVATE ${OpenMP_CXX_FLAGS})
endforeach()
