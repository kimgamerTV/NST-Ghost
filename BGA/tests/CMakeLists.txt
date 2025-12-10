find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

add_executable(BGACoreTests
    test_analyzer_stub.cpp
)

target_link_libraries(BGACoreTests
    PRIVATE
        BGACore
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Test
)

add_test(NAME BGACoreTests COMMAND BGACoreTests)

install(TARGETS BGACoreTests
    RUNTIME DESTINATION bin/tests
)

add_executable(TestRpgAnalyzer
    test_rpg_analyzer.cpp
)

target_link_libraries(TestRpgAnalyzer
    PRIVATE
        BGACore
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Test
)

add_test(NAME TestRpgAnalyzer COMMAND TestRpgAnalyzer)

install(TARGETS TestRpgAnalyzer
    RUNTIME DESTINATION bin/tests
)
