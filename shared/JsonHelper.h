#pragma once

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

class JsonHelper
{
public:
    template <typename T>
    static QString serialize(T qObject)
    {
        return serializeRaw(qObject);
    }

    template <typename T>
    static QByteArray serializeRaw(T qObject)
    {
        const auto mObject {&T::staticMetaObject};
        QJsonObject jObject;
        jObject[QStringLiteral("type")] = qObject.type();
        serialize(mObject, &qObject, jObject);
        return QJsonDocument {jObject}.toJson();
    }

    static void serialize(const QMetaObject* mObject, void* qObject, QJsonObject& jObject);

    template <typename T>
    static std::remove_cv_t<std::remove_reference_t<T>> deserialize(const QJsonObject& jObject)
    {
        using U = std::remove_cv_t<std::remove_reference_t<T>>;
        const auto mObject {&U::staticMetaObject};
        U qObject;
        deserialize(mObject, &qObject, jObject);
        return qObject;
    }

    static void deserialize(const QMetaObject* mObject, void* qObject, const QJsonObject& jObject);
};
