#pragma once

#include <QAbstractTableModel>

#include "Client.h"

class ClientsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Column
    {
        Id,
        Host,
        Port,
        Status,
        ColumnCount,
    };
private:
    struct Item
    {
        quint32 id;
        QString address;
        quint16 port;
        Client::Status status;
    };
public:
    ClientsModel(QObject* parent = nullptr);
    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    void add(Client* client);
    quint32 id(const QModelIndex& index);
private:
    QString mapStatus(Client::Status status) const;
private:
    QList<Item> items;
    QMap<Item*, int> itemsToRows;
};
