cmake_minimum_required(VERSION 3.16)
project(Meipuru VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Add definition to make it works.
# add_definitions(-DTAGLIB_STATIC)

include_directories(
        include
        taglib
        ${CMAKE_BINARY_DIR}/taglib)

add_subdirectory(taglib)

add_library(MeipuruLib
        include/binding.h
        src/binding.cpp)

target_include_directories(MeipuruLib
        PRIVATE
                taglib/taglib
                taglib/taglib/toolkit
                taglib/taglib/mpeg/id3v2
                ${CMAKE_BINARY_DIR}/taglib
        PUBLIC
                include
                .
        )

# Executable written in C for test
add_executable(Meipuru
        bin/main.c
        include/binding.h
        )


target_link_libraries(MeipuruLib tag)

target_link_libraries(Meipuru MeipuruLib)
