cmake_minimum_required(VERSION 3.16)
project(LuaTranslationPlugin LANGUAGES CXX)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network)
find_package(Lua REQUIRED)

add_library(LuaTranslationPlugin SHARED
    luaplugin.h
    luaplugin.cpp
    luatranslationservice.h
    luatranslationservice.cpp
)

target_include_directories(LuaTranslationPlugin PRIVATE 
    ${CMAKE_SOURCE_DIR}/../../QtLingo/include
    ${LUA_INCLUDE_DIR}
)

target_link_libraries(LuaTranslationPlugin PRIVATE 
    Qt${QT_VERSION_MAJOR}::Core 
    Qt${QT_VERSION_MAJOR}::Network
    QtLingo
    ${LUA_LIBRARIES}
)

# Ensure the plugin is built into the plugins directory
set_target_properties(LuaTranslationPlugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)
