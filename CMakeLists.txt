cmake_minimum_required(VERSION 3.13)

project(science-algs
    LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if(MSVC)
    add_compile_options(/W4)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(SOURCES
    src/main.cpp
    src/cnf.cpp
    src/genetic_cnf.cpp
)

add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

target_link_libraries(${PROJECT_NAME} 
    comctl32
    gdi32
    user32
    kernel32
)
