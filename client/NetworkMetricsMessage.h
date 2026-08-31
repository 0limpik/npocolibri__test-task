#pragma once

#include "Message.h"

class NetworkMetricsMessage : public Message
{
    Q_GADGET
    Q_PROPERTY(float bandwidth MEMBER bandwidth)
    Q_PROPERTY(float latency MEMBER latency)
    Q_PROPERTY(float packet_loss MEMBER packetLoss)
public:
    float bandwidth;
    float latency;
    float packetLoss;

    QString type() const override
    {
        return QStringLiteral("NetworkMetrics");
    }
};

Q_DECLARE_METATYPE(NetworkMetricsMessage);
