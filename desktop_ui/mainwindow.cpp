#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Device.h"
#include <QDebug>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_device(Q_NULLPTR),
    m_deviceReady(false)
{
    ui->setupUi(this);

    ui->labelFirstSensor->setPixmap(QPixmap(":/circle_red.png"));
    ui->labelSecondSensor->setPixmap(QPixmap(":/circle_red.png"));
    ui->labelTime->setText("NOT READY");
    connect(ui->buttonReset, SIGNAL(clicked()), this, SLOT(reset()));

    m_device = new Device(this);
    connect(m_device, SIGNAL(SIG_HANDSHAKED()), this, SLOT(showDeviceReady()));
    connect(m_device, SIGNAL(sensorState(int,int)), this, SLOT(updateSensorsState(int,int)));
    connect(m_device, SIGNAL(totalTime(quint64)), this, SLOT(updateTotalTime(quint64)));
    m_device->openDevice();

    m_pollingTimer.setInterval(100);
    m_pollingTimer.setSingleShot(false);
    connect(&m_pollingTimer, SIGNAL(timeout()), m_device, SLOT(requestStatus()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showDeviceReady()
{
    ui->labelTime->setText("READY!");
    m_deviceReady = true;
    if (!m_pollingTimer.isActive()) {
        m_pollingTimer.start();
    }
}

void MainWindow::reset()
{
    m_device->requestReset();
}

void MainWindow::updateSensorsState(int sensor1, int sensor2)
{
    const float voltage1 = 5 * sensor1 / 1024.f;
    const float voltage2 = 5 * sensor2 / 1024.f;
    qDebug() << "update sensors:" << sensor1 << sensor2 << voltage1 << voltage2;
    if (voltage1 > 2.5f) {
        ui->labelFirstSensor->setPixmap(QPixmap(":/circle_green.png"));
    } else {
        ui->labelFirstSensor->setPixmap(QPixmap(":/circle_red.png"));
    }
    if (voltage2 > 2.5f) {
        ui->labelSecondSensor->setPixmap(QPixmap(":/circle_green.png"));
    } else {
        ui->labelSecondSensor->setPixmap(QPixmap(":/circle_red.png"));
    }
}

void MainWindow::updateTotalTime(quint64 msec)
{
    const quint64 sec = msec / 1000;
    const quint64 remains = msec - sec * 1000;
    const QString elapsed = QString("%1:%2").arg(sec).arg(remains, 3, 10, QLatin1Char('0'));
    qDebug() << "update total time, msec:" << msec << elapsed;
    ui->labelTime->setText(elapsed);
}
