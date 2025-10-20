add_executable(BGATranslator
    main.cpp
)

target_link_libraries(BGATranslator
    PRIVATE
        BGACore
        Qt${QT_VERSION_MAJOR}::Core
)
