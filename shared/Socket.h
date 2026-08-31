#pragma once

#include <QTcpSocket>

class IData
{
public:
    virtual qint64 bytesAvailable() = 0;
    virtual qint64 read(char* ptr, qint64 length) = 0;
};

class SocketReader
{
public:
    SocketReader();
    void read(IData& data);
    bool hasMessage();
    QByteArray getMessage();
private:
    void start(IData& data);
    void resume(IData& data);
    void trim();
private:
    char* ptr;
    qint64 size;
    qint64 offset;
    qint64 capacity;
    qint32 messageSize;
};

class SocketData : public IData
{
public:
    SocketData(QTcpSocket* socket);
    qint64 bytesAvailable();
    qint64 read(char* ptr, qint64 length);
private:
    QTcpSocket* socket;
};

class TestData : public IData
{
    char* ptr;
    qint64 length;
public:
    TestData(char* ptr, qint64 offset, qint64 length)
        : ptr {ptr + offset}, length {length}
    {
    }

    qint64 bytesAvailable() override
    {
        return length;
    }

    qint64 read(char* ptr, qint64 length) override
    {
        auto avalible {qMin(length, this->length)};
        std::memcpy(ptr, this->ptr, avalible);
        this->ptr += avalible;
        this->length -= avalible;
        return avalible;
    }
};
