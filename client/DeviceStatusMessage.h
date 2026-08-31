#pragma once

#include "Message.h"

class DeviceStatusMessage : public Message
{
    Q_GADGET
    Q_PROPERTY(qint32 uptime MEMBER uptime)
    Q_PROPERTY(qint8 cpu_usage MEMBER cpuUsage)
    Q_PROPERTY(qint8 memory_usage MEMBER memoryUsage)
public:
    qint32 uptime;
    qint8 cpuUsage;
    qint8 memoryUsage;

    QString type() const override
    {
        return QStringLiteral("DeviceStatus");
    }
};

Q_DECLARE_METATYPE(DeviceStatusMessage);
