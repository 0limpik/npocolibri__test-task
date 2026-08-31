#include "Server.h"

#include <QDebug>
#include <QString>
#include <QTimer>

#include "ServerSession.h"
#include "SessionSocket.h"
#include "Work.h"

TCPServer::
    TCPServer(
        QString host, quint16 port, float reconnectDelay, int messagesCount, SessionSocket* socket,
        ServerSession* session, Work* work
    )
    : host {host}
    , port {port}
    , reconnectDelay {reconnectDelay}
    , messagesCount {messagesCount}
    , socket {socket}
    , session {session}
    , work {work}
{
    connect(socket, &SessionSocket::disconnected, this, &TCPServer::serverDisconnected);
    connect(socket, &SessionSocket::connected, this, &TCPServer::serverConnected);

    socket->connectToHost(host, port);
}

void TCPServer::serverDisconnected()
{
    qDebug() << "disconnected";
    disconnect(session, &ServerSession::onRegistration, this, &TCPServer::sessionOnRegistration);
    disconnect(session, &ServerSession::onStart, this, &TCPServer::sessionOnStart);

    work->stop();

    QTimer::singleShot(/**/ reconnectDelay * 1'000, this, [this]()
    {
        qDebug() << "reconnect";
        socket->connectToHost(host, port);
    });
}

void TCPServer::serverConnected()
{
    qDebug() << "connected";
    connect(session, &ServerSession::onRegistration, this, &TCPServer::sessionOnRegistration);
}

void TCPServer::sessionOnRegistration(const RegistrationMessage& message)
{
    qDebug() << "registered:" << message.id;
    disconnect(session, &ServerSession::onRegistration, this, &TCPServer::sessionOnRegistration);
    connect(session, &ServerSession::onStart, this, &TCPServer::sessionOnStart);
}

void TCPServer::sessionOnStart(const StartMessage& message)
{
    qDebug()
        << "started:" << message.time.toString(Qt::ISODate) //
        << "\ncpu usage:" << message.cpuUsageWarn //
        << "\nmemory usage:" << message.memoryUsageWarn //
        << "\nbandwidth:" << message.bandwidthWarn //
        << "\nlatency:" << message.latencyWarn //
        << "\npacket loss:" << message.packetLossWarn;
    disconnect(session, &ServerSession::onStart, this, &TCPServer::sessionOnStart);
    connect(work, &Work::completed, this, &TCPServer::workCompleted);
    work->setThresholds({
        .cpuUsage = message.cpuUsageWarn,
        .memoryUsage = message.memoryUsageWarn,
        .bandwidth = message.bandwidthWarn,
        .latency = message.latencyWarn,
        .packetLoss = message.packetLossWarn,
    });
    work->start(messagesCount);
}

void TCPServer::workCompleted()
{
    qDebug() << "completed";
}
