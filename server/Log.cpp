#include "Log.h"

#include <QDateTime>

struct LogCallbackEntry
{
    int id;
    LogCallback callback;
};

static QList<LogCallbackEntry> globalLogCallbacks;
static int nextCallbackId {0};

void globalLogCallback(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    for (const auto& entry : globalLogCallbacks)
    {
        if (entry.callback)
        {
            entry.callback(type, context, message);
        }
    }
}

int registerLogCallback(LogCallback callback)
{
    const auto id {++nextCallbackId};
    globalLogCallbacks.append({id, std::move(callback)});
    return id;
}

void unregisterLogCallback(int id)
{
    globalLogCallbacks.removeIf(
        [id](const LogCallbackEntry& entry)
    {
        return entry.id == id;
    });
}
