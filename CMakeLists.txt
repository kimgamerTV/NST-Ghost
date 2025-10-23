cmake_minimum_required(VERSION 3.16)

project(NST VERSION 0.4.2 LANGUAGES CXX)

add_definitions(-DAPP_VERSION=\"${PROJECT_VERSION}\")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools Concurrent Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools Concurrent Network)

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
    mainwindow.h
    menubar.h
    loadprojectdialog.h
    searchdialog.h
    settingsdialog.h
    fontmanagerdialog.h
    searchcontroller.h
    shortcutcontroller.h
    bgadatamanager.h
    translationservicemanager.h
    translationcontextmenu.h
    updatecontroller.h
    resources.qrc
)

# Add BGA as a subdirectory
add_subdirectory(BGA)
# Add QtLingo as a subdirectory
add_subdirectory(QtLingo)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(NST
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
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

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(NST PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Concurrent
    Qt${QT_VERSION_MAJOR}::Network
    BGACore
    QtLingo
)
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
install(TARGETS NST
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES packaging/debian/nst.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES packaging/icons/nst.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(NST)
endif()

include(CPack)

# CPack configuration
set(CPACK_PACKAGE_NAME "NST")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A tool for translating video games.")
set(CPACK_PACKAGE_VENDOR "NST Ghost")
set(CPACK_PACKAGE_CONTACT "you@example.com")

# Linux (ZIP)
set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_FILE_NAME "nst-linux")

# macOS (DMG)
if(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_PACKAGE_FILE_NAME "nst-macos")
endif()

# Windows (ZIP)
if(WIN32)
    set(CPACK_GENERATOR "ZIP")
    set(CPACK_PACKAGE_FILE_NAME "nst-windows")
endif()

