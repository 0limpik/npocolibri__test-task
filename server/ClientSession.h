#pragma once

#include <QObject>

#include "JsonHelper.h"
#include "RegistrationMessage.h"
#include "Session.h"
#include "SessionSocket.h"
#include "Socket.h"
#include "StartMessage.h"

#include <SessionSocket.h>

class ClientSession : public QObject
{
    Q_OBJECT
public:
    ClientSession(SessionSocket* socket, QObject* parent = nullptr)
        : QObject {parent}, socket {socket}
    {
    }
private:
    MESSAGE_REGISTRY(ClientSession, socket)
    MESSAGE_SEND_QUELED(Registration)
    MESSAGE_SEND_QUELED(Start)
private:
private:
    SessionSocket* socket;
};
