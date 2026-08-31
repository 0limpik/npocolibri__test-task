add_executable(
    client
    WIN32
    MACOSX_BUNDLE
    main.cpp
    CLISettings.cpp
    CLISettings.h
    ServerSession.cpp
    ServerSession.h
    Work.cpp
    Work.h
    Server.cpp
    Server.h
    DeviceStatusMessage.h
    LogMessage.h
    NetworkMetricsMessage.h
)

target_include_directories(client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(client PRIVATE shared Qt::Core Qt6::Network)

install(
    TARGETS client
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
qt_generate_deploy_app_script(
    TARGET client
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
