add_executable(BGATranslator
    main.cpp
)

target_link_libraries(BGATranslator
    PRIVATE
        BGACore
        Qt6::Core
)
