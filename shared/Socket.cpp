#include "Socket.h"

SocketReader::SocketReader()
    : ptr {nullptr}, size {0}, offset {0}, capacity {0}, messageSize {0}
{
}

void SocketReader::read(IData& data)
{
    if (offset < size)
    {
        resume(data);
    }
    else
    {
        start(data);
    }
}

bool SocketReader::hasMessage()
{
    // qDebug() << "has:" << "size" << size << "offset" << offset << "message size" << messageSize;
    if (messageSize == 0)
    {
        if (size - offset < sizeof(decltype(messageSize)))
        {
            return false;
        }

        if (offset > capacity / 2)
        {
            trim();
        }

        messageSize = *reinterpret_cast<decltype(messageSize)*>(ptr + offset);
        offset += sizeof(decltype(messageSize));
    }

    if (size - offset < messageSize)
    {
        return false;
    }
    return true;
}

QByteArray SocketReader::getMessage()
{
    // qDebug() << "get:" << "size" << size << "offset" << offset << "message size" << messageSize;
    const auto data {QByteArray(ptr + offset, messageSize)};
    offset += messageSize;
    messageSize = 0;
    return data;
}

void SocketReader::resume(IData& data)
{
    auto available {data.bytesAvailable()};
    // qDebug() << "resume:" << "size" << size << "offset" << offset << "available" << available;
    auto fullSize {size + available};
    if (capacity < fullSize)
    {
        auto newPtr {new char[fullSize]};
        std::memcpy(newPtr, ptr, size);
        delete[] ptr;
        ptr = newPtr;
    }
    data.read(ptr + size, available);
    size = fullSize;
}

void SocketReader::start(IData& data)
{
    auto available {data.bytesAvailable()};
    // qDebug() << "start:" << "size" << size << "offset" << offset << "available" << available;
    if (capacity < available)
    {
        delete[] ptr;
        ptr = new char[available];
        capacity = available;
    }
    offset = 0;
    size = available;
    data.read(ptr, available);
}

void SocketReader::trim()
{
    std::memcpy(ptr, ptr + offset, size - offset);
    size -= offset;
    offset = 0;
}

SocketData::SocketData(QTcpSocket* socket)
    : socket {socket}
{
}

qint64 SocketData::bytesAvailable()
{
    return socket->bytesAvailable();
}

qint64 SocketData::read(char* ptr, qint64 length)
{
    return socket->read(ptr, length);
}
