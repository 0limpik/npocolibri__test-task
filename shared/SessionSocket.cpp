#include "SessionSocket.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QTcpSocket>

#include "Socket.h"

SessionSocket::SessionSocket(QTcpSocket* socket, QObject* parent)
    : QObject {parent}, socket {socket}
{
    connect(socket, &QTcpSocket::disconnected, this, &SessionSocket::socketDisconnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &SessionSocket::socketErrorOccurred);
    connect(socket, &QTcpSocket::connected, this, &SessionSocket::socketConnected);
    connect(socket, &QTcpSocket::bytesWritten, this, &SessionSocket::socketBytesWritten);
    connect(socket, &QTcpSocket::readyRead, this, &SessionSocket::socketReadyRead);
}

void SessionSocket::connectToHost(const QString& host, quint16 port)
{
    isDisconnected = false;
    socket->connectToHost(host, port);
}

void SessionSocket::disconnectFromHost()
{
    socket->disconnectFromHost();
}

void SessionSocket::send(QByteArray data)
{
    const auto size {static_cast<quint32>(data.length())};
    socket->write(reinterpret_cast<const char*>(&size), sizeof(size));
    socket->write(data);
        // qDebug() << size;
}

void SessionSocket::socketConnected()
{
    emit connected();
}

void SessionSocket::socketDisconnected()
{
    emitDisconnected();
}

void SessionSocket::socketErrorOccurred(QAbstractSocket::SocketError error)
{
    emitDisconnected();
}

void SessionSocket::socketReadyRead()
{
    socketReader.read(socketData);
    while (socketReader.hasMessage())
    {
        const auto data {socketReader.getMessage()};
        const auto document {QJsonDocument::fromJson(data)};
        const auto message {document.object()};
        emit this->message(message);
    }
}

void SessionSocket::socketBytesWritten(qint64 bytes)
{
    written += bytes;
    for (auto& callback : callbacks)
    {
        if (callback.end > written)
        {
            continue;
        }
        auto receiver {callback.receiver};
        auto slot {callback.slot};
        (receiver->*slot)();
        callback.end = 0;
    }
    callbacks.removeIf([](auto x)
        {
            return x.end == 0;
        });
    for (auto& lambdaCallback : lambdaCallbacks)
    {
        if (lambdaCallback.end > written)
        {
            continue;
        }
        lambdaCallback.slot();
        lambdaCallback.end = 0;
    }
    lambdaCallbacks.removeIf([](auto x)
        {
            return x.end == 0;
        });
}

void SessionSocket::removeCallback(QObject* object)
{
    for (auto i {0}; i < callbacks.length(); ++i)
    {
        auto& callback {callbacks[i]};
        if (callback.receiver != object)
        {
            continue;
        }
        callbacks.remove(i);
        break;
    }
}

void SessionSocket::emitDisconnected()
{
    if (isDisconnected)
    {
        return;
    }
    emit disconnected();
    isDisconnected = true;
}
