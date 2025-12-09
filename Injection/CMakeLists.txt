cmake_minimum_required(VERSION 3.16)

project(Injection LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core LinguistTools)

set(TS_FILES Injection_en_US.ts)

add_library(Injection SHARED
  Injection_global.h
  injection.cpp
  injection.h
  min_hook/buffer.h
  min_hook/buffer.c
  min_hook/hde/hde32.h
  min_hook/hde/hde32.c
  min_hook/hde/hde64.h
  min_hook/hde/hde64.c
  min_hook/hook.c
  min_hook/trampoline.h
  min_hook/trampoline.c
  min_hook/MinHook.h
  ${TS_FILES}
)

if (WIN32)
    target_link_libraries(Injection PRIVATE Qt${QT_VERSION_MAJOR}::Core ws2_32)
else()
    target_link_libraries(Injection PRIVATE Qt${QT_VERSION_MAJOR}::Core)
endif()

target_compile_definitions(Injection PRIVATE INJECTION_LIBRARY)

if(COMMAND qt_create_translation)
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()
