#pragma once

#include "Message.h"

class RegistrationMessage : public Message
{
    Q_GADGET
    Q_PROPERTY(quint32 id MEMBER id)
public:
    quint32 id {0};

    QString type() const override
    {
        return QStringLiteral("Registration");
    }
};

Q_DECLARE_METATYPE(RegistrationMessage);
