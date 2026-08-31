add_executable(
    server
    WIN32
    MACOSX_BUNDLE
    Client.cpp
    Client.h
    ClientSession.cpp
    ClientSession.h
    ClientsModel.cpp
    ClientsModel.h
    ClientThresholdsDialog.cpp
    ClientThresholdsDialog.h
    ClientThresholdsDialog.ui
    "Log.cpp"
    "Log.h"
    MainWindow.cpp
    MainWindow.h
    MainWindow.ui
    MessagesModel.cpp
    MessagesModel.h
    TCPServer.cpp
    TCPServer.h
    main.cpp
)

target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(server PRIVATE shared Qt::Core Qt::Widgets Qt6::Network)

install(
    TARGETS server
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
qt_generate_deploy_app_script(
    TARGET server
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
