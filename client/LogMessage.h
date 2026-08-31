#pragma once

#include "Message.h"

class LogMessage : public Message
{
    Q_GADGET
    Q_PROPERTY(QString message MEMBER message)
    Q_PROPERTY(Severity severity MEMBER severity)
public:
    enum class Severity
    {
        Info,
        Debug,
        Warn,
        Crit,
        Fatal,
    };
    Q_ENUM(Severity)

    QString message;
    Severity severity;

    QString type() const override
    {
        return QStringLiteral("Log");
    }
};

Q_DECLARE_METATYPE(LogMessage);
