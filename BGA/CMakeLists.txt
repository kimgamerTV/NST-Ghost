cmake_minimum_required(VERSION 3.16)

project(BGA LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core LinguistTools Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core LinguistTools Test)

set(BGA_TS_FILES
    ${CMAKE_SOURCE_DIR}/BGA_en_US.ts
)

add_subdirectory(core)
add_subdirectory(tools/runner)

enable_testing()
add_subdirectory(tests)

if(COMMAND qt_create_translation)
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${BGA_TS_FILES})
else()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${BGA_TS_FILES})
endif()
