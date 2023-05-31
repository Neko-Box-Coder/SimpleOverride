cmake_minimum_required(VERSION 3.14)

if(TARGET SimpleOverride)
    return()
endif()

set (CMAKE_CXX_STANDARD 11)

# For Clang to do parsing
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set the project name
project(SimpleOverrideProject)

# ==========================================================
# Variables 
# ==========================================================
set(SIMPLE_OVERRIDE_BUILD_TEST "ON" CACHE BOOL "Build tests for simple override")

add_library(SimpleOverride INTERFACE)
target_include_directories(SimpleOverride INTERFACE "${CMAKE_CURRENT_LIST_DIR}")
target_sources(SimpleOverride INTERFACE "${CMAKE_CURRENT_LIST_DIR}/FunctionOverrides.hpp")

if(SIMPLE_OVERRIDE_BUILD_TEST)
    add_executable(SimpleOverrideTest "${CMAKE_CURRENT_LIST_DIR}/Tests/FunctionOverridesTests.cpp")
    target_link_libraries(SimpleOverrideTest PUBLIC SimpleOverride)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/External/ssTest" "${CMAKE_CURRENT_BINARY_DIR}/ssTest")
    target_link_libraries(SimpleOverrideTest PUBLIC ssTest)
endif()

