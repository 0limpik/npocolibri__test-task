#include "Client.h"

#include <QDebug>
#include <QObject>
#include <QString>

#include "ClientSession.h"
#include "SessionSocket.h"

Client::
    Client(
        quint32 id, const QHostAddress& address, quint16 port, SessionSocket* socket, ClientSession* session,
        QObject* parent
    )
    : QObject {parent}
    , m_id {id}
    , m_address {address}
    , m_port {port}
    , socket {socket}
    , session {session}
    , m_status {Status::Connected}
{
    connect(socket, &SessionSocket::disconnected, this, &Client::socketDisconnected);
}

void Client::registration()
{
    setStatus(Status::Registration);
    RegistrationMessage message;
    message.id = id();
    session->Registration(message, this, &Client::sessionRegistration);
}

void Client::start(Thresholds thresholds)
{
    setStatus(Status::Starting);
    StartMessage message;
    message.time = QDateTime::currentDateTimeUtc();
    message.cpuUsageWarn = thresholds.cpuUsage;
    message.memoryUsageWarn = thresholds.memoryUsage;
    message.bandwidthWarn = thresholds.bandwidth;
    message.latencyWarn = thresholds.latency;
    message.packetLossWarn = thresholds.packetLossPercent / 100;
    session->Start(message, this, &Client::sessionStart);
}

void Client::disconnect_()
{
    setStatus(Status::Disconnected);
    QMetaObject::invokeMethod(/**/ socket, &SessionSocket::disconnectFromHost, Qt::QueuedConnection);
}

void Client::socketDisconnected()
{
    setStatus(Status::Disconnected);
    disconnect(socket, &SessionSocket::message, this, &Client::sessionMessage);
    QMetaObject::invokeMethod(/**/ this, &Client::disconnected, Qt::QueuedConnection);
    this->deleteLater();
}

void Client::sessionRegistration()
{
    setStatus(Status::Registred);
    QMetaObject::invokeMethod(/**/ this, &Client::registred, Qt::QueuedConnection);
}

void Client::sessionStart()
{
    setStatus(Status::Working);
    connect(socket, &SessionSocket::message, this, &Client::sessionMessage);
    QMetaObject::invokeMethod(/**/ this, &Client::started, Qt::QueuedConnection);
}

void Client::sessionMessage(const QJsonObject& object)
{
    const QString type {object[QStringLiteral("type")].toString()};
    QJsonObject content {object};
    content.remove(QStringLiteral("type"));
    QMetaObject::invokeMethod(/**/ this, [this, type, object]
    {
        emit message(type, object);
    }, Qt::QueuedConnection);
}

void Client::setStatus(Status status)
{
    m_status = status;
    QMetaObject::invokeMethod(/**/ this, &Client::statusChanged, Qt::QueuedConnection);
}
