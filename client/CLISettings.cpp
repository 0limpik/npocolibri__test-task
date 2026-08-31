#include "CLISettings.h"

#include <QCommandLineParser>
#include <QCoreApplication>

CLISettings::CLISettings(const QCoreApplication& application)
{
    // clang-format off
    parse(application, {
        {
            QCommandLineOption {"host", "Server hostname or IP address", "string", "localhost"},
            [this](QString v, bool s) { host = v; }
        },
        {
            QCommandLineOption {"port", "Server port number", "integer", "12345"},
            [this](QString v, bool s) { port = v.toUShort(); }
        },
        {
            QCommandLineOption {"reconnect-delay", "Delay in seconds before retrying a failed server connection", "float", "5"},
            [this](QString v, bool s) { reconnectDelay = v.toFloat(); }
        },
        {
            QCommandLineOption {"message-delay-min", "Minimum delay in seconds between debug messages sent to server", "float", "0.01"},
            [this](QString v, bool s) { messageDelayMin = v.toFloat(); }
        },
        {
            QCommandLineOption {"message-delay-max", "Maximum delay in seconds between debug messages sent to server", "float", "0.1"},
            [this](QString v, bool s) { messageDelayMax = v.toFloat(); }
        },
        {
            QCommandLineOption {"message-delay-none", "Disable delay between debug messages sent to server",},
            [this](QString v, bool s) { messageDelayNone = s; }
        },
        {
            QCommandLineOption {"messages-count", "Disable delay between debug messages sent to server", "integer", "-1"},
            [this](QString v, bool s) { messagesCount = v.toInt(); }
        },
    });
    // clang-format on
}

void CLISettings::parse(const QCoreApplication& application, std::initializer_list<CLISetting> options)
{
    QCommandLineParser parser;
    parser.setApplicationDescription("NPO Colibri Test Task Client");
    parser.addHelpOption();
    parser.addVersionOption();

    for (const auto& setting : options)
    {
        parser.addOption(setting.option);
    }

    parser.process(application);

    for (const auto& setting : options)
    {
        auto isSet {parser.isSet(setting.option)};
        auto value {parser.value(setting.option)};
            // qDebug() << setting.option.valueName() << isSet << value << setting.option.defaultValues();
        setting.setValue(value, isSet);
    }
}
