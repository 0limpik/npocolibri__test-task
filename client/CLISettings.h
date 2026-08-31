#pragma once

#include <QCommandLineOption>
#include <QString>

class QCoreApplication;

struct CLISetting
{
    using SetValue = std::function<void(QString value, bool isSet)>;

    QCommandLineOption option;
    SetValue setValue;
};

struct CLISettings
{
    QString host;
    quint16 port;
    float reconnectDelay;

    bool messageDelayNone;
    float messageDelayMin;
    float messageDelayMax;
    int messagesCount;

    CLISettings(const QCoreApplication& application);
private:
    void parse(const QCoreApplication& application, std::initializer_list<CLISetting> options);
};
