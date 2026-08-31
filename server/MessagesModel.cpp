#include "MessagesModel.h"

MessagesModel::MessagesModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int MessagesModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return items.size();
}

QVariant MessagesModel::data(const QModelIndex& index, int role) const
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
            case ClientId: return item.clientId;
            case Type: return item.type;
            case Content: return item.content;
            case Time: return item.time.toString("yyyy-MM-dd hh:mm:ss.zzz AP");
            default: return {};
        }
    }
    return {};
}

QVariant MessagesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
    {
        return {};
    }
    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
            case ClientId: return QStringLiteral("Client ID");
            case Type: return QStringLiteral("Type");
            case Content: return QStringLiteral("Content");
            case Time: return QStringLiteral("Time");
        }
    }
    return section + 1;
}

void MessagesModel::add(Item item)
{
    const auto row {items.size()};
    beginInsertRows({}, row, row);
    items.append(item);
    endInsertRows();
}

int MessagesModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return ColumnCount;
}
