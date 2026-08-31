#pragma once

#include <functional>
#include <QList>
#include <QString>

using LogCallback = std::function<void(QtMsgType type, const QMessageLogContext& context, const QString& message)>;

void globalLogCallback(QtMsgType type, const QMessageLogContext& context, const QString& message);

int registerLogCallback(LogCallback callback);
void unregisterLogCallback(int id);
