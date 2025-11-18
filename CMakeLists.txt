cmake_minimum_required(VERSION 3.16)
project(NST VERSION 0.4.6 LANGUAGES CXX)
add_definitions(-DAPP_VERSION=\"${PROJECT_VERSION}\")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fix Linux RPATH to find libraries in same directory
if(UNIX AND NOT APPLE)
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
    set(CMAKE_BUILD_RPATH "$ORIGIN")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools Concurrent Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools Concurrent Network)
find_package(Lua)

# Use qt_standard_project_setup for Qt 6.3+
if(QT_VERSION_MAJOR EQUAL 6 AND QT_VERSION_MINOR GREATER_EQUAL 3)
    qt_standard_project_setup()
endif()

set(TS_FILES NST_en_US.ts)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    menubar.cpp
    loadprojectdialog.cpp
    searchdialog.cpp
    settingsdialog.cpp
    searchcontroller.cpp
    shortcutcontroller.cpp
    bgadatamanager.cpp
    translationservicemanager.cpp
    translationcontextmenu.cpp
    updatecontroller.cpp
    fontmanagerdialog.cpp
    pluginmanagerdialog.cpp
    projectdatamanager.cpp # New file
    mainwindow.h
    menubar.h
    loadprojectdialog.h
    searchdialog.h
    settingsdialog.h
    fontmanagerdialog.h
    pluginmanagerdialog.h
    searchcontroller.h
    shortcutcontroller.h
    bgadatamanager.h
    translationservicemanager.h
    translationcontextmenu.h
    updatecontroller.h
    customprogressdialog.cpp
    customprogressdialog.h
    projectdatamanager.h # New file
    resources.qrc
)

# Lua plugin sources (optional)
if(Lua_FOUND)
    list(APPEND PROJECT_SOURCES
        src/plugins/LuaPlugin.cpp
        src/plugins/LuaPlugin.h
        src/plugins/LuaScriptManager.cpp
        src/plugins/LuaScriptManager.h
        src/plugins/PluginManager.cpp
        src/plugins/PluginManager.h
    )
endif()

# Add BGA as a subdirectory
add_subdirectory(BGA)
# Add QtLingo as a subdirectory
add_subdirectory(QtLingo)
# Add plugins subdirectory
add_subdirectory(plugins)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(NST
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
    qt_create_translation(QM_FILES ${PROJECT_SOURCES} ${TS_FILES})
else()
    if(ANDROID)
        add_library(NST SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(NST
            ${PROJECT_SOURCES}
        )
    endif()
    qt5_create_translation(QM_FILES ${PROJECT_SOURCES} ${TS_FILES})
endif()

target_link_libraries(NST PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Concurrent
    Qt${QT_VERSION_MAJOR}::Network
    BGACore
    QtLingo
)

if(Lua_FOUND)
    target_link_libraries(NST PRIVATE ${LUA_LIBRARIES})
    target_include_directories(NST PRIVATE ${LUA_INCLUDE_DIR})
    target_compile_definitions(NST PRIVATE HAS_LUA=1)
endif()

target_include_directories(NST PRIVATE ${CMAKE_SOURCE_DIR}/BGA/core/include ${CMAKE_SOURCE_DIR}/QtLingo/include)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.NST)
endif()

set_target_properties(NST PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)

# Install executable
install(TARGETS NST
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install BGACore and QtLingo libraries
if(UNIX AND NOT APPLE)
    install(TARGETS BGACore QtLingo
        LIBRARY DESTINATION ${CMAKE_INSTALL_BINDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

if(WIN32)
    install(TARGETS BGACore QtLingo
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()

install(FILES packaging/debian/nst.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES packaging/icons/nst.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)

# Option to skip Qt deployment (useful for AppImage/custom packaging)
option(SKIP_QT_DEPLOY "Skip Qt deployment during install" OFF)

# Use Qt's official deployment API (Qt 6.3+)
if(QT_VERSION_MAJOR EQUAL 6 AND QT_VERSION_MINOR GREATER_EQUAL 3 AND NOT SKIP_QT_DEPLOY)
    qt_generate_deploy_app_script(
        TARGET NST
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${deploy_script})
else()
    # Fallback for older Qt versions or when SKIP_QT_DEPLOY is ON
    if(WIN32 AND NOT SKIP_QT_DEPLOY)
        # Find windeployqt
        get_target_property(_qmake_executable Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
        get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
        find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
        
        if(WINDEPLOYQT_EXECUTABLE)
            message(STATUS "Found windeployqt: ${WINDEPLOYQT_EXECUTABLE}")
            
            # Install Qt DLLs with CPack
            install(CODE "
                execute_process(
                    COMMAND \"${WINDEPLOYQT_EXECUTABLE}\"
                        --release
                        --no-translations
                        --no-system-d3d-compiler
                        --no-opengl-sw
                        --compiler-runtime
                        --dir \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}\"
                        \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/NST.exe\"
                    RESULT_VARIABLE windeployqt_result
                    OUTPUT_VARIABLE windeployqt_output
                    ERROR_VARIABLE windeployqt_error
                )
                
                if(NOT windeployqt_result EQUAL 0)
                    message(WARNING \"windeployqt failed: \${windeployqt_error}\")
                else()
                    message(STATUS \"windeployqt output: \${windeployqt_output}\")
                endif()
            " COMPONENT Runtime)
            
            # Create qt.conf to ensure Qt finds plugins
            install(CODE "
                set(qt_conf_content \"[Paths]\\nPlugins = plugins\\nPrefix = .\\n\")
                file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/qt.conf\" \"\${qt_conf_content}\")
                message(STATUS \"Created qt.conf in \${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}\")
            " COMPONENT Runtime)
        else()
            message(WARNING "windeployqt not found. Qt dependencies will not be deployed automatically.")
        endif()
        
        # Install MinGW Runtime (if using MinGW)
        if(MINGW)
            get_filename_component(MINGW_BIN_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
            
            set(MINGW_DLLS
                "libgcc_s_seh-1.dll"
                "libstdc++-6.dll"
                "libwinpthread-1.dll"
            )
            
            foreach(dll ${MINGW_DLLS})
                set(dll_path "${MINGW_BIN_DIR}/${dll}")
                if(EXISTS "${dll_path}")
                    install(FILES "${dll_path}"
                        DESTINATION ${CMAKE_INSTALL_BINDIR}
                        COMPONENT Runtime
                    )
                    message(STATUS "Will install MinGW runtime: ${dll}")
                else()
                    message(WARNING "MinGW runtime not found: ${dll_path}")
                endif()
            endforeach()
        endif()
        
        # Install Visual C++ Runtime (if using MSVC)
        if(MSVC)
            set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
            include(InstallRequiredSystemLibraries)
            install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
                DESTINATION ${CMAKE_INSTALL_BINDIR}
                COMPONENT Runtime
            )
        endif()
    endif()
    
    # macOS: Bundle Qt frameworks
    if(APPLE AND NOT SKIP_QT_DEPLOY)
        get_target_property(_qmake_executable Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
        get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
        find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")
        
        if(MACDEPLOYQT_EXECUTABLE)
            message(STATUS "Found macdeployqt: ${MACDEPLOYQT_EXECUTABLE}")
            
            add_custom_command(TARGET NST POST_BUILD
                COMMAND "${MACDEPLOYQT_EXECUTABLE}"
                    "$<TARGET_BUNDLE_DIR:NST>"
                    -always-overwrite
                COMMENT "Deploying Qt frameworks with macdeployqt..."
            )
        else()
            message(WARNING "macdeployqt not found. Qt frameworks will not be bundled automatically.")
        endif()
    endif()
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(NST)
endif()

# CPack configuration
set(CPACK_PACKAGE_NAME "NST")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A tool for translating video games.")
set(CPACK_PACKAGE_VENDOR "NST Ghost")
set(CPACK_PACKAGE_CONTACT "you@example.com")

# Default generator
set(CPACK_GENERATOR "ZIP")

# Linux
if(UNIX AND NOT APPLE)
    set(CPACK_PACKAGE_FILE_NAME "NST-${PROJECT_VERSION}-Linux")
endif()

# macOS (DMG)
if(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_PACKAGE_FILE_NAME "NST-${PROJECT_VERSION}-macOS")
endif()

# Windows (ZIP)
if(WIN32)
    set(CPACK_PACKAGE_FILE_NAME "NST-${PROJECT_VERSION}-win64")
endif()

include(CPack)
