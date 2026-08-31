#pragma once

#include <QObject>

#define MESSAGE_SEND(Name) \
public: \
    void Name(Name##Message message) \
    { \
        __getSocket()->send(JsonHelper::serializeRaw(message)); \
    } \
    template <typename Receiver> \
    void Name(Name##Message message, Receiver* receiver, void (Receiver::*slot)()) \
    { \
        __getSocket()->send(JsonHelper::serializeRaw(message), receiver, slot); \
    }

#define MESSAGE_SEND_QUELED(Name) \
public: \
    void Name(Name##Message message) \
    { \
        QMetaObject::invokeMethod(/**/ this, [this, message] \
        { \
            __getSocket()->send(JsonHelper::serializeRaw(message)); \
        }, Qt::QueuedConnection); \
    } \
    template <typename Receiver> \
    void Name(Name##Message message, Receiver* receiver, void (Receiver::*slot)()) \
    { \
        QMetaObject::invokeMethod(/**/ this, [this, message, receiver, slot] \
        { \
            __getSocket()->send(/**/ JsonHelper::serializeRaw(message), receiver, [receiver, slot] \
            { \
                QMetaObject::invokeMethod(/**/ receiver, [receiver, slot] \
                { \
                    (receiver->*slot)(); \
                }, Qt::QueuedConnection); \
            }); \
        }, Qt::QueuedConnection); \
    }

#define MESSAGE_RECEIVE(Name) \
public: \
Q_SIGNALS: \
    void on##Name(const Name##Message& message); \
private: \
    inline static const bool __registered##Name { \
        __Registry::registerMessage<const Name##Message&, &__SelfType::on##Name>(#Name) \
    };

#define MESSAGE_REGISTRY(Name, Socket) \
private: \
    using __SelfType = Name; \
    SessionSocket* __getSocket() \
    { \
        return Socket; \
    } \
    struct __Registry \
    { \
        using ParserFunc = std::function<void(__SelfType*, const QJsonObject&)>; \
        static QHash<QString, ParserFunc>& get() \
        { \
            static QHash<QString, ParserFunc> instance; \
            return instance; \
        } \
        template <typename MessageType, void (__SelfType::*SignalFunc)(const MessageType&)> \
        static bool registerMessage(const QString& name) \
        { \
            get()[name] = [](__SelfType* instance, const QJsonObject& object) \
            { \
                Q_EMIT(instance->*SignalFunc)(JsonHelper::deserialize<const MessageType&>(object)); \
            }; \
            return true; \
        } \
    };
