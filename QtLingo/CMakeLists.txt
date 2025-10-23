cmake_minimum_required(VERSION 3.16)

project(QtLingo LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core LinguistTools Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core LinguistTools Network)

set(TS_FILES QtLingo_en_US.ts)

add_library(QtLingo SHARED
  QtLingo_global.h
  qtlingo.cpp
  qtlingo.h
  include/qtlingo/translationservice.h
  include/qtlingo/translationservicefactory.h
  src/translationservicefactory.cpp
  src/google_translate_service.h
  src/google_translate_service.cpp
  src/llm_translation_service.h
  src/llm_translation_service.cpp
  ${TS_FILES}
)

target_include_directories(QtLingo
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(QtLingo PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network)

target_compile_definitions(QtLingo PRIVATE QTLINGO_LIBRARY)

if(COMMAND qt_create_translation)
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()
