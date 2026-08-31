#include <QCoreApplication>

#include "CLISettings.h"
#include "Server.h"
#include "ServerSession.h"
#include "SessionSocket.h"
#include "Work.h"

int main(int argc, char* argv[])
{
    QCoreApplication application {argc, argv};

    CLISettings settings {application};

    SessionSocket socket {new QTcpSocket, &application};
    ServerSession session {&socket, &application};
    Work work {settings.messageDelayMin, settings.messageDelayMax, settings.messageDelayNone, &session};
    TCPServer server {
        settings.host, settings.port, settings.reconnectDelay, settings.messagesCount, &socket, &session, &work,
    };

    return QCoreApplication::exec();
}
