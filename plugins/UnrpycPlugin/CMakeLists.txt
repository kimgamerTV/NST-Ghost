cmake_minimum_required(VERSION 3.16)
project(UnrpycPlugin)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

add_library(UnrpycPlugin SHARED
    UnrpycPlugin.cpp
    UnrpycPlugin.h
)

target_link_libraries(UnrpycPlugin PRIVATE Qt6::Core Qt6::Widgets)
target_include_directories(UnrpycPlugin PRIVATE ${CMAKE_SOURCE_DIR}/src)
