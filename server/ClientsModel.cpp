#include "ClientsModel.h"

ClientsModel::ClientsModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int ClientsModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return items.size();
}

int ClientsModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return ColumnCount;
}

QVariant ClientsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
    {
        return {};
    }
    auto& item {items.at(index.row())};
    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
            case Id: return item.id;
            case Host: return item.address;
            case Port: return item.port;
            case Status: return mapStatus(item.status);
            default: return {};
        }
    }
    return {};
}

QVariant ClientsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
    {
        return {};
    }
    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
            case Id: return QStringLiteral("ID");
            case Host: return QStringLiteral("Host");
            case Port: return QStringLiteral("Port");
            case Status: return QStringLiteral("Status");
            default: return {};
        }
    }
    return section + 1;
}

void ClientsModel::add(Client* client)
{
    const auto row {items.size()};
    beginInsertRows({}, row, row);
    auto& item {
            items.emplace_back(
                Item {
                    .id = client->id(),
                    .address = client->address().toString(),
                    .port = client->port(),
                    .status = client->status(),
                }
            )
        };
    itemsToRows[&item] = row;
    connect(/**/ client, &Client::statusChanged, this, [this, client, row]()
    {
        items[row].status = client->status();
        emit dataChanged(index(row, Column::Status), index(row, Column::Status), {Qt::DisplayRole});
    });
    endInsertRows();
}

quint32 ClientsModel::id(const QModelIndex& index)
{
    return items[index.row()].id;
}

QString ClientsModel::mapStatus(Client::Status status) const
{
    using S = Client::Status;
    switch (status)
    {
        case S::Connected: return QStringLiteral("Connected");
        case S::Registration: return QStringLiteral("Registration");
        case S::Registred: return QStringLiteral("Registred");
        case S::Starting: return QStringLiteral("Starting");
        case S::Working: return QStringLiteral("Working");
        case S::Disconnected: return QStringLiteral("Disconnected");
    }
    return QStringLiteral("?");
}
