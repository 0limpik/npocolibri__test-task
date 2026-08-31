#include "ClientThresholdsDialog.h"

#include "Client.h"
#include "ui_ClientThresholdsDialog.h"

ClientThresholdsDialog::ClientThresholdsDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::ClientThresholdsDialog)
{
    ui->setupUi(this);
#define CREATE_SLIDER_LINK(name) \
    m_thresholds.name = ui->name##_slider->value(); \
    connect(/**/ ui->name##_slider, &QSlider::valueChanged, this, [this](int value) \
    { \
        ui->name##_value->setText(QString::number(value)); \
        m_thresholds.name = value; \
    });
    CREATE_SLIDER_LINK(cpuUsage);
    CREATE_SLIDER_LINK(memoryUsage);
    CREATE_SLIDER_LINK(bandwidth);
    CREATE_SLIDER_LINK(latency);
    CREATE_SLIDER_LINK(packetLossPercent);
}

ClientThresholdsDialog::~ClientThresholdsDialog()
{
    delete ui;
}

void ClientThresholdsDialog::open(const Client::Thresholds& thresholds)
{
    ui->cpuUsage_slider->setValue(thresholds.cpuUsage);
    ui->memoryUsage_slider->setValue(thresholds.memoryUsage);
    ui->bandwidth_slider->setValue(thresholds.bandwidth);
    ui->latency_slider->setValue(thresholds.latency);
    ui->packetLossPercent_slider->setValue(thresholds.packetLossPercent);
    show();
}

