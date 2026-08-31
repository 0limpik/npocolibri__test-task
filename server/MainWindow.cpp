#include "MainWindow.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QScrollBar>
#include <QThread>

#include "ClientsModel.h"
#include "ClientThresholdsDialog.h"
#include "Log.h"
#include "MessagesModel.h"
#include "TCPServer.h"
#include "ui_MainWindow.h"

namespace
{
    Client::Thresholds defaultThresholds {
        .cpuUsage = 95,
        .memoryUsage = 95,
        .bandwidth = 125,
        .latency = 200,
        .packetLossPercent = 1,
    };
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow {parent}, ui {new Ui::MainWindow}
{
    ui->setupUi(this);

    registerLogCallback([this](QtMsgType type, const QMessageLogContext& context, const QString& message)
    {
        log(type,context, message);
    });

    clientsModel = new ClientsModel {this};
    ui->clients->setModel(clientsModel);

    messagesModel = new MessagesModel {this};
    ui->messages->setModel(messagesModel);
    auto messagesHeader {ui->messages->horizontalHeader()};
    for (auto i {0}; i < messagesHeader->count(); ++i)
    {
        if (i == MessagesModel::Content)
        {
            messagesHeader->setSectionResizeMode(i, QHeaderView::Stretch);
        }
        else
        {
            messagesHeader->setSectionResizeMode(i, QHeaderView::ResizeToContents);
        }
    }

    connect(/**/ ui->clients->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            [this](const QItemSelection& selected, const QItemSelection& deselected)
    {
        updateClientButtons(selected.indexes());
    });
    connect(/**/ clientsModel, &QAbstractItemModel::dataChanged, this,
            [this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles)
    {
        const auto selection {ui->clients->selectionModel()->selection()};
        for (const auto& range : selection)
        {
            if (range.bottom() < topLeft.row() || range.top() > bottomRight.row())
            {
                continue;
            }
            updateClientButtons(selection.indexes());
            return;
        }
    });

    connect(/**/ ui->start, &QPushButton::clicked, this, [this]()
    {
        const auto selection {ui->clients->selectionModel()->selection()};
        for (const auto& index : selection.indexes())
        {
            if (index.column() != 0)
            {
                continue;
            }
            auto id {clientsModel->id(index)};
            auto client {clients[id].client};
            connect(/**/ client, &Client::started, this, [this, id]()
            {
                qDebug() << "started:" << id;
            });
            client->start(clients[id].thresholds);
        }
    });
    connect(/**/ ui->stop, &QPushButton::clicked, this, [this]()
    {
        const auto selection {ui->clients->selectionModel()->selection()};
        for (const auto& index : selection.indexes())
        {
            if (index.column() != 0)
            {
                continue;
            }
            auto id {clientsModel->id(index)};
            auto client {clients[id].client};
            connect(/**/ client, &Client::disconnected, this, [this, id]()
            {
                qDebug() << "stoped:" << id;
            });
            client->disconnect_();
        }
    });
    connect(/**/ ui->clients, &QTableView::doubleClicked, this, [this](const QModelIndex& index)
    {
        auto id {clientsModel->id(index)};
        if (!clients.contains(id))
        {
            return;
        }

        auto dialog {new ClientThresholdsDialog {}};
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        connect(/**/ dialog, &ClientThresholdsDialog::accepted, this, [this, id, dialog]()
        {
            clients[id].thresholds = dialog->thresholds();
        });
        dialog->open(clients[id].thresholds);
    });

    server = new TCPServer {this};
    connect(/**/ server, &TCPServer::connection, this, [this](Client* client)
    {
        auto id {client->id()};
        qDebug() << "connected:" << id;
        clientsModel->add(client);
        clients[id] = ClientData {
            .client = client,
            .thresholds = defaultThresholds,
        };
        connect(/**/ client, &Client::disconnected, this, [this, id]()
        {
            clients.remove(id);
        });
        connect(/**/ client, &Client::registred, this, [this, client]()
        {
            qDebug() << "registered:" << client->id();
            connect(/**/ client, &Client::message, this, [this, client](const QString& type, const QJsonObject& content)
            {
                auto pretty {content};
                for (const auto& key : pretty.keys())
                {
                    const auto& value {pretty[key]};
                    if (value.isDouble())
                    {
                        const auto rounded {std::round(value.toDouble() * 100) / 100};
                        pretty[key] = rounded;
                    }
                }
                messagesModel->add({
                    .clientId = client->id(),
                    .type = type,
                    .content = QJsonDocument {pretty}.toJson(QJsonDocument::JsonFormat::Compact),
                    .time = QDateTime::currentDateTime(),
                });
            });
        });
        client->registration();
    });
    connect(/**/ messagesModel, &QAbstractItemModel::rowsInserted, this, [this]()
    {
        auto bar {ui->messages->verticalScrollBar()};
        if (bar->value() == bar->maximum())
        {
            ui->messages->scrollToBottom();
        }
    });
    connect(/**/ server, &TCPServer::listening, this, [this](bool result, QString error)
    {
        if (!result)
        {
            qWarning() << "server failed" << error;
        }
        else
        {
            qDebug() << "server started";
        }
    });
    server->listen(12'345);
    return;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateClientButtons(const QModelIndexList& indexes)
{
    ui->start->setEnabled(/**/ std::any_of(/**/ indexes.cbegin(), indexes.cend(), /**/ [this](const QModelIndex& index)
    {
        auto id {clientsModel->id(index)};
        if (!clients.contains(id))
        {
            return false;
        }
        return clients[id].client->status() == Client::Status::Registred;
    }));

    ui->stop->setEnabled(/**/ std::any_of(/**/ indexes.cbegin(), indexes.cend(), /**/ [this](const QModelIndex& index)
    {
        auto id {clientsModel->id(index)};
        if (!clients.contains(id))
        {
            return false;
        }
        return clients[id].client->status() != Client::Status::Disconnected;
    }));
}

void MainWindow::log(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    QString typeStr;
    switch (type)
    {
        case QtDebugMsg: typeStr = QStringLiteral("<font color='#808080'>[DEBUG]</font>"); break;
        case QtInfoMsg: typeStr = QStringLiteral("<font color='#4A90E2'>[INFO ]</font>"); break;
        case QtWarningMsg: typeStr = QStringLiteral("<font color='#D4A017'>[WARN ]</font>"); break;
        case QtCriticalMsg: typeStr = QStringLiteral("<font color='#D64545'>[CRIT ]</font>"); break;
        case QtFatalMsg: typeStr = QStringLiteral("<font color='#C02020'><b>[FATAL]</b></font>"); break;
    }

    auto timestamp {QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz AP")};
    auto thread {QThread::currentThread()};
    auto formatted {
        QString("0x%1 %2 %3 %4").arg(reinterpret_cast<quintptr>(thread), 0, 16).arg(timestamp).arg(typeStr).arg(message)
    };
    QMetaObject::invokeMethod(/**/ this, [this, formatted]()
    {
        ui->log->append(formatted);
    }, Qt::QueuedConnection);
}
