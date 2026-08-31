#include "ServerSession.h"

ServerSession::ServerSession(SessionSocket* socket, QObject* parent)
    : QObject {parent}, socket {socket}
{
    connect(socket, &SessionSocket::message, this, &ServerSession::socketMessage);
}

void ServerSession::socketMessage(const QJsonObject& object)
{
    auto type {object["type"].toString()};
    auto& registry {__Registry::get()};
    if (registry.contains(type))
    {
        registry[type](this, object);
    }
    else
    {
        qWarning() << "unknow object: " << object;
    }
}
