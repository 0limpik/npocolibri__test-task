#pragma once

#include <QObject>
#include <QRandomGenerator>

class ServerSession;

class Work : public QObject
{
    Q_OBJECT
public:
    struct Thresholds
    {
        quint8 cpuUsage {101};
        quint8 memoryUsage {101};
        float bandwidth {std::numeric_limits<float>::max()};
        float latency {std::numeric_limits<float>::max()};
        float packetLoss {std::numeric_limits<float>::max()};
    };

    Work(float minDelay, float maxDelay, bool noDelay, ServerSession* session);
    void start(int workCount);
    void stop();
    void setThresholds(Thresholds thresholds);
signals:
    void completed();
private:
    void syncWork();
    void delayedWork();
    void sendRandomMessage();
    void sendThresholdLog(const QString& type);
private:
    ServerSession* session;
    QRandomGenerator generator;

    float minDelay;
    float maxDelay;
    bool noDelay;
    Thresholds thresholds;

    int workCount {0};
    bool isWorking {false};

    const static QList<QString> shortMessages;
    const static QString chars;
};
