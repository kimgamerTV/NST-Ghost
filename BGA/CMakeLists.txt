cmake_minimum_required(VERSION 3.16)

project(BGACore LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

# =============================================================================
# Rust Library Build (bga_rs)
# =============================================================================
set(BGA_RS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bga_rs)
if(MSVC)
    set(BGA_RS_LIBRARY ${BGA_RS_DIR}/target/release/bga_rs.lib)
else()
    set(BGA_RS_LIBRARY ${BGA_RS_DIR}/target/release/libbga_rs.a)
endif()

# Custom command to build the Rust library
add_custom_command(
    OUTPUT ${BGA_RS_LIBRARY}
    COMMAND cargo build --release
    WORKING_DIRECTORY ${BGA_RS_DIR}
    COMMENT "Building Rust BGA library..."
    VERBATIM
)

# Custom target for Rust build
add_custom_target(bga_rust ALL DEPENDS ${BGA_RS_LIBRARY})

# =============================================================================
# Source files (C++ with Rust bridge)
# =============================================================================
set(CORE_SOURCES
    core/src/analyzerfactory.cpp
    core/src/bga_rust_bridge.cpp
    # Legacy C++ implementations (can be removed once Rust is fully tested)
    core/src/engines/rpgm/rpganalyzer.cpp
    core/src/engines/unity/unityanalyzer.cpp
    core/src/engines/renpy/renpyanalyzer.cpp
)

set(CORE_HEADERS
    core/include/core/analyzerfactory.h
    core/include/core/gameanalyzer.h
    core/include/core/bga_rust_bridge.h
    core/include/core/engines/rpgm/rpganalyzer.h
    core/include/core/engines/unity/unityanalyzer.h
    core/include/core/engines/renpy/renpyanalyzer.h
)

# =============================================================================
# Create library
# =============================================================================
add_library(BGACore SHARED ${CORE_SOURCES} ${CORE_HEADERS})

# Ensure Rust is built before BGACore
add_dependencies(BGACore bga_rust)

# Export symbols on Windows
set_target_properties(BGACore PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)

# =============================================================================
# Linking
# =============================================================================
target_link_libraries(BGACore PRIVATE 
    Qt${QT_VERSION_MAJOR}::Core
    ${BGA_RS_LIBRARY}
)

# Platform-specific linking for Rust static library
if(UNIX AND NOT APPLE)
    target_link_libraries(BGACore PRIVATE pthread dl m)
elseif(APPLE)
    target_link_libraries(BGACore PRIVATE pthread dl)
elseif(WIN32)
    target_link_libraries(BGACore PRIVATE ws2_32 userenv bcrypt ntdll)
endif()

target_include_directories(BGACore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core/include
)

add_subdirectory(tests)
