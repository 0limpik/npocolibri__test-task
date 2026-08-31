#pragma once

#include <QObject>
#include <QRandomGenerator>

class Client;
class QTcpServer;

class TCPServer : public QObject
{
    Q_OBJECT
public:
    TCPServer(QObject* parent = nullptr);
    ~TCPServer();
    void listen(quint16 port);
signals:
    void listening(bool result, QString error);
    void connection(Client* client);
private slots:
    void serverNewConnection();
private:
    QRandomGenerator idGenerator;
    QTcpServer* server;
    QThread* thread;
};
