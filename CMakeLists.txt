cmake_minimum_required(VERSION 3.11)

project(wstl VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译类型
set(CMAKE_BUILD_TYPE "Release")

# 显示编译器信息
message(STATUS "Compiler： ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # 使用g++编译器
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wextra -Wno-sign-compare -Wno-unused-variable")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0.0)
        message(FATAL_ERROR "Require at least gcc-5.0")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif()

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    # 使用clang++编译器
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wextra -Wno-sign-compare -Wno-unused-variable -Wno-array-bounds")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.5.0)
        message(FATAL_ERROR "Require at least clang-3.5")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif()

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # 使用MSVC编译器
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /wd4100 /wd4127 /wd4201 /wd4324 /wd4456 /wd4457 /wd4458 /wd4459 /wd4702 /wd4996")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.0)
        message(FATAL_ERROR "Require at least MSVC-2015")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++14")
    endif()

endif()

message(STATUS "The CXX flags: ${CMAKE_CXX_FLAGS}")

add_subdirectory(${PROJECT_SOURCE_DIR}/wstl)
add_subdirectory(${PROJECT_SOURCE_DIR}/test)