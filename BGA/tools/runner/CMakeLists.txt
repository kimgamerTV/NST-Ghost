add_executable(BGARunner
    main.cpp
)

target_link_libraries(BGARunner
    PRIVATE
        BGACore
        Qt6::Core
)

install(TARGETS BGARunner
    RUNTIME DESTINATION bin
)
