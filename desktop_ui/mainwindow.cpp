#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Device.h"
#include "testprecision.h"

#include <QAction>
#include <QDebug>
#include <QPixmap>
#include <QKeySequence>

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

    QAction *actionTestPrecision = new QAction("Open test form", Q_NULLPTR);
    actionTestPrecision->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
    ui->buttonTestPrecision->addAction(actionTestPrecision);
    connect(ui->buttonTestPrecision, SIGNAL(clicked(bool)), this, SLOT(showPrecisionTestingForm()));
    connect(ui->buttonTestPrecision, SIGNAL(triggered(QAction*)), this, SLOT(showPrecisionTestingForm()));

    m_pollingTimer.setInterval(1000);
    m_pollingTimer.setSingleShot(false);
    connect(&m_pollingTimer, SIGNAL(timeout()), m_device, SLOT(requestStatus()));

    connect(ui->sliderFirst, SIGNAL(sliderMoved(int)), this, SLOT(sliderMovedFirst(int)));
    connect(ui->sliderSecond, SIGNAL(sliderMoved(int)), this, SLOT(sliderMovedSecond(int)));
    float threshold1 = ui->sliderFirst->value() / 10.f;
    float threshold2 = ui->sliderSecond->value() / 10.f;
    ui->labelFirst->setText(QString("1: %1").arg(threshold1, 0, 'f', 1, '0'));
    ui->labelSecond->setText(QString("2: %1").arg(threshold2, 0, 'f', 1, '0'));
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
    float threshold1 = ui->sliderFirst->value() / 10.f;
    float threshold2 = ui->sliderSecond->value() / 10.f;
    if (voltage1 > threshold1) {
        ui->labelFirstSensor->setPixmap(QPixmap(":/circle_green.png"));
    } else {
        ui->labelFirstSensor->setPixmap(QPixmap(":/circle_red.png"));
    }
    if (voltage2 > threshold2) {
        ui->labelSecondSensor->setPixmap(QPixmap(":/circle_green.png"));
    } else {
        ui->labelSecondSensor->setPixmap(QPixmap(":/circle_red.png"));
    }
    int cur1 = 50 * sensor1 / 1024;
    int cur2 = 50 * sensor2 / 1024;
    ui->sliderFirstCurrent->setValue(cur1);
    ui->sliderSecondCurrent->setValue(cur2);
}

void MainWindow::updateTotalTime(quint64 msec)
{
    const quint64 sec = msec / 1000;
    const quint64 remains = msec - sec * 1000;
    const QString elapsed = QString("%1:%2").arg(sec).arg(remains, 3, 10, QLatin1Char('0'));
    qDebug() << "update total time, msec:" << msec << elapsed;
    ui->labelTime->setText(elapsed);
}

void MainWindow::sliderMovedFirst(int value)
{
    float threshold1 = value / 10.f;
    ui->labelFirst->setText(QString("1: %1").arg(threshold1, 0, 'f', 1, '0'));
    m_device->sendThresholdFirst(value);
}

void MainWindow::sliderMovedSecond(int value)
{
    float threshold2 = value / 10.f;
    ui->labelSecond->setText(QString("2: %1").arg(threshold2, 0, 'f', 1, '0'));
    m_device->sendThresholdSecond(value);
}

void MainWindow::showPrecisionTestingForm()
{
    TestPrecision *test = new TestPrecision();
    test->open();
}
