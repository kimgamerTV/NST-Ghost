add_executable(BGARunner
    main.cpp
)

target_link_libraries(BGARunner
    PRIVATE
        BGACore
        Qt${QT_VERSION_MAJOR}::Core
)

install(TARGETS BGARunner
    RUNTIME DESTINATION bin
)
