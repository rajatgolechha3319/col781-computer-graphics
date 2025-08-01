cmake_minimum_required(VERSION 3.0)

project(a4)
set(CMAKE_CXX_STANDARD 11)

find_package(glm REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

add_library(col781 src/base/hw.cpp src/base/camera.cpp src/frame/rectangle.cpp src/frame/catmull_rom.cpp src/frame/cloth.cpp deps/src/gl.c)
target_include_directories(col781 PUBLIC deps/include)
target_link_libraries(col781 glm::glm OpenGL::GL SDL2::SDL2)
target_compile_options(col781 PRIVATE -O3 -Wall)

add_executable(part1 executables/part1.cpp)
target_include_directories(part1 PUBLIC src)
target_link_libraries(part1 col781)
target_compile_options(part1 PRIVATE -O3 -Wall)

add_executable(part2 executables/part2.cpp)
target_include_directories(part2 PUBLIC src)
target_link_libraries(part2 col781)
target_compile_options(part2 PRIVATE -O3 -Wall)

add_executable(part3 executables/part3.cpp)
target_include_directories(part3 PUBLIC src)
target_link_libraries(part3 col781)
target_compile_options(part3 PRIVATE -O3 -Wall)