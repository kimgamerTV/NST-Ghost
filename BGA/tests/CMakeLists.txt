find_package(Qt6 REQUIRED COMPONENTS Test)

add_executable(BGACoreTests
    test_analyzer_stub.cpp
)

target_link_libraries(BGACoreTests
    PRIVATE
        BGACore
        Qt6::Core
        Qt6::Test
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
        Qt6::Core
        Qt6::Test
)

add_test(NAME TestRpgAnalyzer COMMAND TestRpgAnalyzer)

install(TARGETS TestRpgAnalyzer
    RUNTIME DESTINATION bin/tests
)
