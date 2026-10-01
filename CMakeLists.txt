# Lock CMake Required VERSION
cmake_minimum_required(VERSION 3.20) 

project(bluff_game LANGUAGES CXX)

# set C++ standard 
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# linking libraries 
add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)

find_package(SDL3 REQUIRED)

add_executable(bluff src/main.cpp)

target_include_directories(bluff PUBLIC include)
target_link_libraries(bluff PRIVATE server_lib client_lib SDL3::SDL3)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if(MSVC)
    target_compile_options(bluff PRIVATE /W4)
else()
    target_compile_options(bluff PRIVATE -Wall -Wextra -Wpedantic)
endif()

