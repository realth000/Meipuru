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

# Meipuru Library
add_library(MeipuruLib
        SHARED
        src/MeipuruExport.h
        bin/main.cpp
        include/MeipuruReader.h
        src/MeipuruReader.cpp
        src/BaseTag.h
        src/BaseTag.cpp
        src/ID3v2Tag.h
        src/ID3v2Tag.cpp
        src/Utils.h)

# Executable for test
add_executable(Meipuru
        bin/main.cpp
        include/MeipuruReader.h)

set_target_properties(MeipuruLib PROPERTIES
        DEFINE_SYMBOL MEIPURU_LIB)


target_link_libraries(MeipuruLib tag)

# If using MingW, add zlib target as required by TagLib.
target_link_libraries(Meipuru MeipuruLib)
