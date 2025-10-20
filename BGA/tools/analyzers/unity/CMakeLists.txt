add_executable(BGAAnalyzerUnity
    main.cpp
    unityanalyzer.h
    unityanalyzer.cpp
)

target_link_libraries(BGAAnalyzerUnity
    PRIVATE
        BGACore
        Qt${QT_VERSION_MAJOR}::Core
)

install(TARGETS BGAAnalyzerUnity
    RUNTIME DESTINATION bin
)
