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
