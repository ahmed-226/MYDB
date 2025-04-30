cmake_minimum_required(VERSION 3.10)
project(mydb C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

include_directories(include)

file(GLOB SOURCES "src/*.c")

add_executable(mydb ${SOURCES})

install(TARGETS mydb DESTINATION bin)