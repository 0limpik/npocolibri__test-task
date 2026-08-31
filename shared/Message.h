#pragma once

#include <QObject>

class Message
{
    Q_GADGET
public:
    virtual ~Message() = default;
    virtual QString type() const = 0;
};
