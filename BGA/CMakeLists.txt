cmake_minimum_required(VERSION 3.16)

project(BGACore LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

# Source files
set(CORE_SOURCES
    core/src/analyzerfactory.cpp
    core/src/engines/rpgm/rpganalyzer.cpp
    core/src/engines/unity/unityanalyzer.cpp
    core/src/engines/renpy/renpyanalyzer.cpp
)

set(CORE_HEADERS
    core/include/core/analyzerfactory.h
    core/include/core/gameanalyzer.h
    core/include/core/engines/rpgm/rpganalyzer.h
    core/include/core/engines/unity/unityanalyzer.h
    core/include/core/engines/renpy/renpyanalyzer.h
)

# Create library
add_library(BGACore SHARED ${CORE_SOURCES} ${CORE_HEADERS})

#  export symbols ทั้งหมดบน Windows
set_target_properties(BGACore PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)

target_link_libraries(BGACore PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_include_directories(BGACore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core/include
)
