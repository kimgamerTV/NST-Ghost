cmake_minimum_required(VERSION 3.16)
project(LuaTranslationPlugin LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Network)
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
    Qt6::Core 
    Qt6::Network
    QtLingo
    ${LUA_LIBRARIES}
)

# Ensure the plugin is built into the plugins directory
set_target_properties(LuaTranslationPlugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)
