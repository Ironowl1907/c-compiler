cmake_minimum_required(VERSION 3.10)
project(MyCProject LANGUAGES C)

set(SOURCES 
    ./src/main.c
		./src/lexer.c
)

include_directories(include)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})
