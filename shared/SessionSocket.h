#pragma once

#include <QObject>
#include <QTcpSocket>

#include "Socket.h"

class SessionSocket : public QObject
{
    Q_OBJECT
public:
    SessionSocket(QTcpSocket* socket, QObject* parent = nullptr);
    void connectToHost(const QString& host, quint16 port);
    void disconnectFromHost();
    void send(QByteArray data);

    template <typename Receiver>
    void send(QByteArray data, Receiver* receiver, void (Receiver::*slot)())
    {
        connect(receiver, &QObject::destroyed, this, &SessionSocket::removeCallback);
        const auto size {static_cast<quint32>(data.length())};
        callbacks.emplace_back(written + size, receiver, static_cast<void (QObject::*)()>(slot));
        socket->write(reinterpret_cast<const char*>(&size), sizeof(size));
        socket->write(data);
        callbacks.emplace_back(written + size, receiver, static_cast<void (QObject::*)()>(slot));
    }

    template <typename Receiver, typename Func>
    void send(QByteArray data, Receiver* receiver, Func&& callback)
    {
        connect(receiver, &QObject::destroyed, this, &SessionSocket::removeCallback);
        const auto size {static_cast<quint32>(data.length())};
        lambdaCallbacks.emplace_back(written + size, receiver, std::forward<Func>(callback));
        socket->write(reinterpret_cast<const char*>(&size), sizeof(size));
        socket->write(data);
    }

signals:
    void connected();
    void message(const QJsonObject& object);
    void disconnected();
private:
    void socketConnected();
    void socketDisconnected();
    void socketErrorOccurred(QAbstractSocket::SocketError error);
    void socketReadyRead();
    void socketBytesWritten(qint64 bytes);
    void removeCallback(QObject* object);
    void emitDisconnected();
private:
    QTcpSocket* socket;
    SocketData socketData {socket};
    SocketReader socketReader;
    struct Callback
    {
        qint64 end;
        QObject* receiver;
        void (QObject::*slot)();

        Callback(qint64 end, QObject* receiver, void (QObject::*slot)())
            : end {end}, receiver {receiver}, slot {slot}
        {
        }
    };
    QList<Callback> callbacks;
    struct LambdaCallback
    {
        qint64 end;
        QObject* receiver;
        std::function<void()> slot;

        LambdaCallback(qint64 end, QObject* receiver, std::function<void()> slot)
            : end {end}, receiver {receiver}, slot {slot}
        {
        }
    };
    QList<LambdaCallback> lambdaCallbacks;
    qint64 written;
    bool isDisconnected {false};
};
