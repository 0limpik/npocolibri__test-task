#pragma once

#include <QDateTime>

#include "Message.h"

class StartMessage : public Message
{
    Q_GADGET
    Q_PROPERTY(QDateTime time MEMBER time)
    Q_PROPERTY(quint8 cpu_usage_warn MEMBER cpuUsageWarn)
    Q_PROPERTY(quint8 memory_usage_warn MEMBER memoryUsageWarn)
    Q_PROPERTY(float bandwidth_warn MEMBER bandwidthWarn)
    Q_PROPERTY(float latency_warn MEMBER latencyWarn)
    Q_PROPERTY(float packet_loss_warn MEMBER packetLossWarn)
public:
    QDateTime time;
    quint8 cpuUsageWarn;
    quint8 memoryUsageWarn;
    float bandwidthWarn;
    float latencyWarn;
    float packetLossWarn;

    QString type() const override
    {
        return QStringLiteral("Start");
    }
};

Q_DECLARE_METATYPE(StartMessage);
