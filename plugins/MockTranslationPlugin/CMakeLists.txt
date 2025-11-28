cmake_minimum_required(VERSION 3.16)
project(MockTranslationPlugin LANGUAGES CXX)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(MockTranslationPlugin SHARED
    mockplugin.h
    mockplugin.cpp
)

target_link_libraries(MockTranslationPlugin PRIVATE Qt${QT_VERSION_MAJOR}::Core QtLingo)

# Ensure the plugin is built into the plugins directory
set_target_properties(MockTranslationPlugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)
