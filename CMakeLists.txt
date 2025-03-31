cmake_minimum_required(VERSION 3.21)

project(fardroid CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(fardroid SHARED Config.cpp fardroid.cpp fardroidPlugin.cpp FARstuffs.cpp framebuffer.cpp myrtl.cpp MyStr.cpp stdafx.cpp stuffs.cpp taskbarIcon.cpp ThreadPool.cpp fardroid.rc)
target_include_directories(fardroid PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/includes)
target_compile_definitions(fardroid PRIVATE _UNICODE __STDC_LIMIT_MACROS)
target_link_options(fardroid PRIVATE /DEF:${CMAKE_CURRENT_SOURCE_DIR}/fardroid.def)
target_link_libraries(fardroid PRIVATE Ws2_32)
