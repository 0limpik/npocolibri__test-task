#include "Work.h"

#include <QCoreApplication>
#include <QMetaEnum>
#include <QObject>
#include <QRandomGenerator>
#include <QTimer>

#include "DeviceStatusMessage.h"
#include "LogMessage.h"
#include "NetworkMetricsMessage.h"
#include "ServerSession.h"

Work::Work(float minDelay, float maxDelay, bool noDelay, ServerSession* session)
    : minDelay {minDelay}, maxDelay {maxDelay}, noDelay {noDelay}, session {session}
{
}

void Work::start(int workCount)
{
    isWorking = true;
    this->workCount = workCount;
    if (noDelay)
    {
        syncWork();
    }
    else
    {
        delayedWork();
    }
}

void Work::stop()
{
    isWorking = false;
}

void Work::setThresholds(Thresholds thresholds)
{
    this->thresholds = thresholds;
}

void Work::syncWork()
{
    while (--workCount > 0 || workCount == -1)
    {
        if (!isWorking)
        {
            return;
        }
        sendRandomMessage();
    }

    isWorking = false;
    emit completed();
}

void Work::delayedWork()
{
    if (workCount != -1 && --workCount <= 0)
    {
        isWorking = false;
        emit completed();
        return;
    }

    auto delay {minDelay + generator.bounded(maxDelay - minDelay)};
    QTimer::singleShot(/**/ delay * 1'000, this, [this]()
    {
        if (!isWorking)
        {
            return;
        }
        sendRandomMessage();
        delayedWork();
    });
}

void Work::sendRandomMessage()
{
    const auto messageType {generator.bounded(3)};
    if (messageType == 0)
    {
        auto cpuUsage {generator.bounded(101)};
        auto memoryUsage {generator.bounded(101)};

        DeviceStatusMessage message;
        message.uptime = generator.bounded(3'600 * 24);
        message.cpuUsage = cpuUsage;
        message.memoryUsage = memoryUsage;
        session->DeviceStatus(message);

        if (cpuUsage > thresholds.cpuUsage)
        {
            sendThresholdLog(QStringLiteral("cpuUsage"));
        }
        if (memoryUsage > thresholds.memoryUsage)
        {
            sendThresholdLog(QStringLiteral("memoryUsage"));
        }
    }
    else if (messageType == 1)
    {
        auto bandwidth {generator.bounded(200.0f)};
        auto latency {1.0f + generator.bounded(199.0f)};
        auto packetLoss {generator.bounded(0.1f)};
        NetworkMetricsMessage message;
        message.bandwidth = bandwidth;
        message.latency = latency;
        message.packetLoss = packetLoss;
        session->NetworkMetrics(message);
        if (bandwidth > thresholds.bandwidth)
        {
            sendThresholdLog(QStringLiteral("bandwidth"));
        }
        if (latency > thresholds.latency)
        {
            sendThresholdLog(QStringLiteral("latency"));
        }
        if (packetLoss > thresholds.packetLoss)
        {
            sendThresholdLog(QStringLiteral("packetLoss"));
        }
    }
    else
    {
        LogMessage message;
        const auto logMessageType {generator.bounded(40)};
        if (logMessageType == 0)
        {
            const auto length {generator.bounded(200, 5'000)};
            QString result;
            result.reserve(length);
            for (auto i {0}; i < length; ++i)
            {
                const auto index {generator.bounded(static_cast<int>(Work::chars.length()))};
                result.append(Work::chars.at(index));
            }
            message.message = std::move(result);
        }
        else
        {
            message.message = Work::shortMessages[generator.bounded(Work::shortMessages.length())];
        }
        const auto metaEnum {QMetaEnum::fromType<LogMessage::Severity>()};
        const auto index {generator.bounded(metaEnum.keyCount())};
        message.severity = static_cast<LogMessage::Severity>(metaEnum.value(index));
        session->Log(message);
    }
}

void Work::sendThresholdLog(const QString& type)
{
    LogMessage message;
    message.severity = LogMessage::Severity::Warn;
    message.message = QStringLiteral("Thresholds exceeded for %1").arg(type);
    session->Log(message);
}

const QList<QString> Work::shortMessages {
    QString {
        R"(Initializing secure handshake... 0x88FF21
Establishing encrypted tunnel at 128.0.0.1:8080
Scanning network nodes... [OK]
Buffer overflow protection: ENABLED
7b 22 75 73 65 72 22 3a 20 22 61 64 6d 69 6e 22 7d
Overriding kernel permissions... [SUCCESS]
Unrecognized packet signature detected at 0x0045AF
Synchronizing database mirrors... 42% complete
Decrypting partition /dev/sda1... [||||------]
Access granted: UID 0, GID 0
a1 b2 c3 d4 e5 f6 07 18 29 3a 4b 5c 6d 7e 8f 90
Uplink established. Monitoring telemetry...
Cleaning temporary trace files... [DELETED]
Operation complete. Terminal idle.)"
    }
        .split("\n")
};

const QString Work::chars {
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
};
