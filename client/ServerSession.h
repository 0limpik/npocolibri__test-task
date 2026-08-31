#pragma once

#include <QObject>

#include "DeviceStatusMessage.h"
#include "JsonHelper.h"
#include "LogMessage.h"
#include "NetworkMetricsMessage.h"
#include "RegistrationMessage.h"
#include "Session.h"
#include "SessionSocket.h"
#include "Socket.h"
#include "StartMessage.h"

class ServerSession : public QObject
{
    Q_OBJECT
public:
    ServerSession(SessionSocket* socket, QObject* parent);
private:
    MESSAGE_REGISTRY(ServerSession, socket)
    MESSAGE_SEND(Log)
    MESSAGE_SEND(DeviceStatus)
    MESSAGE_SEND(NetworkMetrics)
    MESSAGE_RECEIVE(Registration)
    MESSAGE_RECEIVE(Start)
private:
    void socketMessage(const QJsonObject& object);
private:
    SessionSocket* socket;
};
