cmake_minimum_required(VERSION 3.05)
project(Meipuru VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Add definition to make it works.
add_definitions(-DTAGLIB_STATIC)

include_directories(taglib/taglib
        taglib/taglib/toolkit
        taglib/taglib/mpeg/id3v2
        ${CMAKE_BINARY_DIR}/taglib)

add_subdirectory(taglib)

add_executable(Meipuru
        src/main.cpp
        src/MeipuruReader.h
        src/MeipuruReader.cpp
        src/BaseTag.h
        src/BaseTag.cpp
        src/ID3v2Tag.h
        src/ID3v2Tag.cpp
        src/Utils.h)


# If using MingW, add zlib target as required by TagLib.
target_link_libraries(Meipuru tag)
