#include "JsonHelper.h"

#include <QMetaProperty>

void JsonHelper::serialize(const QMetaObject* mObject, void* qObject, QJsonObject& jObject)
{
    for (auto i {0}; i < mObject->propertyCount(); ++i)
    {
        const auto mProperty {mObject->property(i)};
        const auto mPropertyName {QString {mProperty.name()}};
        const auto qValue {mProperty.readOnGadget(qObject)};
        QJsonValue jValue;
        if (qValue.metaType().id() == QMetaType::QDateTime)
        {
            jValue = qValue.toDateTime().toUTC().toString(Qt::ISODate);
        }
        else if (mProperty.isEnumType())
        {
            const auto metaEnum {mProperty.enumerator()};
            if (metaEnum.isValid())
            {
                const auto enumValue {qValue.toInt()};
                const auto key {metaEnum.valueToKey(enumValue)};
                if (key != nullptr)
                {
                    jValue = QString::fromUtf8(key);
                }
                else
                {
                    jValue = qValue.toJsonValue();
                }
            }
            else
            {
                jValue = qValue.toJsonValue();
            }
        }
        else
        {
            jValue = qValue.toJsonValue();
        }
        jObject[mPropertyName] = jValue;
    }
}

void JsonHelper::deserialize(const QMetaObject* mObject, void* qObject, const QJsonObject& jObject)
{
    for (auto i {0}; i < mObject->propertyCount(); ++i)
    {
        const auto mProperty {mObject->property(i)};
        const auto mPropertyName {QString {mProperty.name()}};
        const auto jProperty {jObject[mPropertyName]};
        mProperty.writeOnGadget(qObject, jProperty.toVariant());
    }
}
