#include "TCPServer.h"

#include <QTcpServer>
#include <QThread>

#include "Client.h"
#include "ClientSession.h"
#include "SessionSocket.h"
#include "Socket.h"

TCPServer::TCPServer(QObject* parent)
    : QObject {parent}
{
    thread = new QThread {this};
    server = new QTcpServer;
    server->moveToThread(thread);
    connect(server, &QTcpServer::newConnection, this, &TCPServer::serverNewConnection);
    thread->start();
}

void TCPServer::listen(quint16 port)
{
    QMetaObject::invokeMethod(/**/ server, [this, port]
    {
        auto result {server->listen(QHostAddress::Any, port)};
        auto error {result ? QStringLiteral("") : server->errorString()};
        QMetaObject::invokeMethod(/**/ this, [this, result, error]
        {
            emit listening(result, error);
        }, Qt::QueuedConnection);
    }, Qt::QueuedConnection);
}

void TCPServer::serverNewConnection()
{
    auto id {static_cast<quint32>(idGenerator.bounded(100, 999))};
    auto connection {server->nextPendingConnection()};
    QMetaObject::invokeMethod(/**/ connection, [this, connection, id]
    {
        auto socket {new SessionSocket {connection}};
        auto session {new ClientSession {socket, socket}};
        emit socket->connected();
        QMetaObject::invokeMethod(/**/ this, [this, id, connection, socket, session]
        {
            auto client {new Client {id, connection->peerAddress(), connection->peerPort(), socket, session, this}};
            emit this->connection(client);
        }, Qt::QueuedConnection);
    }, Qt::QueuedConnection);
}

TCPServer::~TCPServer()
{
    thread->quit();
    thread->wait();
}
