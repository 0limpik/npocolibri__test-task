#pragma once

#include <QObject>

class SessionSocket;
class ServerSession;
class Work;
class RegistrationMessage;
class StartMessage;

class TCPServer : public QObject
{
    Q_OBJECT
public:
    TCPServer(//
        QString host,
        quint16 port,
        float reconnectDelay,
        int messagesCount,
        SessionSocket* socket,
        ServerSession* session,
        Work* work
    );
private:
    void serverDisconnected();
    void serverConnected();
    void sessionOnRegistration(const RegistrationMessage& message);
    void sessionOnStart(const StartMessage& message);
    void workCompleted();
private:
    SessionSocket* socket;
    ServerSession* session;

    QString host;
    quint16 port;
    float reconnectDelay;
    int messagesCount;

    Work* work;
};
