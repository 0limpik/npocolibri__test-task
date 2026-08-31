#pragma once

#include <QAbstractTableModel>
#include <QDateTime>

class MessagesModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Column
    {
        ClientId,
        Type,
        Content,
        Time,
        ColumnCount,
    };

    struct Item
    {
        quint32 clientId;
        QString type;
        QString content;
        QDateTime time;
    };

    MessagesModel(QObject* parent = nullptr);
    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    void add(Item item);
private:
    QList<Item> items;
};
