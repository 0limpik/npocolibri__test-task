#pragma once

#include <QHostAddress>
#include <QObject>

class SessionSocket;
class ClientSession;

class Client : public QObject
{
    Q_OBJECT
public:
    enum class Status
    {
        Connected,
        Registration,
        Registred,
        Starting,
        Working,
        Disconnected,
    };

    struct Thresholds
    {
        quint8 cpuUsage;
        quint8 memoryUsage;
        float bandwidth;
        float latency;
        float packetLossPercent;
    };

    Client(quint32 id, const QHostAddress& address, quint16 port, SessionSocket* socket, ClientSession* session, QObject* parent = nullptr);

    const Status& status()
    {
        return m_status;
    }

    const quint32& id() const
    {
        return m_id;
    }

    const QHostAddress& address() const
    {
        return m_address;
    }

    const quint16& port() const
    {
        return m_port;
    }

    void registration();
    void start(Thresholds thresholds);
    void disconnect_();
signals:
    void registred();
    void started();
    void message(const QString& type, const QJsonObject& content);
    void statusChanged();
    void disconnected();
private:
    void socketDisconnected();
    void sessionRegistration();
    void sessionStart();
    void sessionMessage(const QJsonObject& object);
    void setStatus(Status status);
private:
    SessionSocket* socket;
    ClientSession* session;

    Status m_status;
    quint32 m_id;
    QHostAddress m_address;
    quint16 m_port;
};
