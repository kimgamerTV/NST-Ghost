cmake_minimum_required(VERSION 3.16)
project(MockTranslationPlugin LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core)

add_library(MockTranslationPlugin SHARED
    mockplugin.h
    mockplugin.cpp
)

target_link_libraries(MockTranslationPlugin PRIVATE Qt6::Core QtLingo)

# Ensure the plugin is built into the plugins directory
set_target_properties(MockTranslationPlugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)
