cmake_minimum_required(VERSION 3.25)

project(MultithreadCounter)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_SUPPRESS_REGENERATION true)

set(SOURCE_FILES MultithreadCounter.cpp Main.cpp)
set(HEADER_FILES MultithreadCounter.h)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

