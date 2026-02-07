cmake_minimum_required(VERSION 3.10)
project(c-compiler LANGUAGES C)

set(SOURCES 
    src/main.c
    src/lexer.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${PROJECT_NAME} PRIVATE 
        -g 
        -Wall 
        -Wextra 
        -O0 
				-Wpedantic
    )
endif()

include_directories(include)
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
