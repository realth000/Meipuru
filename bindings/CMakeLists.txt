cmake_minimum_required(VERSION 3.16)
project(Meipuru VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(TAGLIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/taglib")
set(BINDINGS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")

if(MSVC)
        if(DEFINED MEIPURU_BUILD_DYNAMIC_LIBS)
                # Dynamic linking.
                set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
        else()
                # Static linking.
                set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
        endif()

        add_compile_options(/MP)
endif()

include_directories(
        ${BINDINGS_DIR}/include
        ${TAGLIB_DIR}
        ${CMAKE_BINARY_DIR}/taglib)

if(DEFINED MEIPURU_BUILD_DYNAMIC_LIBS)
        set(BUILD_SHARED_LIBS ON)
else()
        set(BUILD_SHARED_LIBS OFF)
endif()

add_subdirectory(${BINDINGS_DIR}/taglib)

add_library(MeipuruLib
        ${BINDINGS_DIR}/include/binding.h
        ${BINDINGS_DIR}/src/binding.cpp)

target_include_directories(MeipuruLib
        PRIVATE
                ${TAGLIB_DIR}/taglib
                ${TAGLIB_DIR}/taglib/toolkit
                ${TAGLIB_DIR}/taglib/mpeg/id3v2
                ${CMAKE_BINARY_DIR}/taglib
        PUBLIC
                ${BINDINGS_DIR}/include
                ${BINDINGS_DIR}/.
        )

# # Executable written in C for test
# add_executable(Meipuru
#         ${BINDINGS_DIR}/bin/main.c
#         ${BINDINGS_DIR}/include/binding.h
#         )

target_link_libraries(MeipuruLib PRIVATE tag)

# target_link_libraries(Meipuru MeipuruLib)
