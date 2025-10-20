add_executable(BGAAnalyzerRpgm
    main.cpp
    rpganalyzer.h
    rpganalyzer.cpp
)

target_link_libraries(BGAAnalyzerRpgm
    PRIVATE
        BGACore
        Qt${QT_VERSION_MAJOR}::Core
)

install(TARGETS BGAAnalyzerRpgm
    RUNTIME DESTINATION bin
)
