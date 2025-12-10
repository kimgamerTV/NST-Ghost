cmake_minimum_required(VERSION 3.16)
project(MockDataPlugin)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

add_library(MockDataPlugin SHARED
    MockDataPlugin.cpp
    MockDataPlugin.h
)

target_link_libraries(MockDataPlugin PRIVATE Qt6::Core Qt6::Widgets)
target_include_directories(MockDataPlugin PRIVATE ${CMAKE_SOURCE_DIR}/src)
