cmake_minimum_required(VERSION 3.5)

project(MyChip8Emulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(SDL2 REQUIRED)

add_executable(MyChip8Emulator main.cpp cpu.cpp logger.cpp)
target_link_libraries(MyChip8Emulator PRIVATE SDL2::SDL2)
