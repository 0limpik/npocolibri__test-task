add_library(
    shared
    STATIC
    JsonHelper.cpp
    JsonHelper.h
    Session.h
    SessionSocket.cpp
    SessionSocket.h
    Socket.cpp
    Socket.h
    Message.h
    RegistrationMessage.h
    StartMessage.h
)

target_include_directories(shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(shared PRIVATE Qt::Core Qt::Network)
