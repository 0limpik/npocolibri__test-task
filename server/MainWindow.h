#pragma once

#include <QMainWindow>
#include <QModelIndexList>

#include "Client.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class TCPServer;
class ClientsModel;
class MessagesModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
private:
    void log(QtMsgType type, const QMessageLogContext& context, const QString& message);
    void updateClientButtons(const QModelIndexList& indexes);
private:
    Ui::MainWindow* ui;

    struct ClientData
    {
        Client* client;
        Client::Thresholds thresholds;
    };

    TCPServer* server;
    QMap<quint32, ClientData> clients;
    ClientsModel* clientsModel;
    MessagesModel* messagesModel;
    int logCallbackId;
};
