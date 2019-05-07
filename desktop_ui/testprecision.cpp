#include "testprecision.h"
#include "ui_testprecision.h"

#include <QDebug>
#include <QPalette>

TestPrecision::TestPrecision(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestPrecision)
{
    ui->setupUi(this);

    m_timerTest.setSingleShot(true);
    m_timerTest.setInterval(1000);

    m_timerReset.setSingleShot(true);
    m_timerReset.setInterval(1000);

    connect(ui->buttonStartTest, SIGNAL(clicked(bool)), this, SLOT(startTest()));
    connect(&m_timerTest, SIGNAL(timeout()), this, SLOT(stopTest()));
    connect(&m_timerReset, SIGNAL(timeout()), this, SLOT(resetLights()));

    ui->lightFirst->setAutoFillBackground(true);
    ui->lightSecond->setAutoFillBackground(true);
    QPalette pallete = ui->lightFirst->palette();
    pallete.setColor(QPalette::Active, QPalette::Window, Qt::white);
    pallete.setColor(QPalette::Inactive, QPalette::Window, Qt::white);
    pallete.setColor(QPalette::Disabled, QPalette::Window, Qt::black);
    ui->lightFirst->setPalette(pallete);
    ui->lightSecond->setPalette(pallete);

    resetLights();

    qDebug() << "create TestPrecision dialog";
}

TestPrecision::~TestPrecision()
{
    delete ui;
}

void TestPrecision::startTest()
{
    m_timerTest.start();
    ui->lightFirst->setEnabled(false);
}

void TestPrecision::stopTest()
{
    ui->lightSecond->setEnabled(false);
    m_timerReset.start();
}

void TestPrecision::resetLights()
{
    ui->lightFirst->setEnabled(true);
    ui->lightSecond->setEnabled(true);
}
