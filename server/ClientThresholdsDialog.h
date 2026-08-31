#pragma once

#include <QDialog>

#include "Client.h"

namespace Ui
{
    class ClientThresholdsDialog;
}

class ClientThresholdsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ClientThresholdsDialog(QWidget* parent = nullptr);
    ~ClientThresholdsDialog();
    void open(const Client::Thresholds& thresholds);

    const Client::Thresholds& thresholds()
    {
        return m_thresholds;
    }
private:
    Client::Thresholds m_thresholds;
    Ui::ClientThresholdsDialog* ui;
};
