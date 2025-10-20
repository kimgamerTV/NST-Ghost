add_executable(BGAAnalyzerTemplate
    main.cpp
    templateanalyzer.h
    templateanalyzer.cpp
)

target_link_libraries(BGAAnalyzerTemplate
    PRIVATE
        BGACore
        Qt${QT_VERSION_MAJOR}::Core
)

install(TARGETS BGAAnalyzerTemplate
    RUNTIME DESTINATION bin
)
