#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class Device;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = Q_NULLPTR);
    ~MainWindow();

private slots:
    void showDeviceReady();
    void reset();
    void updateSensorsState(int sensor1, int sensor2);
    void updateTotalTime(quint64 msec);
    void sliderMovedFirst(int value);
    void sliderMovedSecond(int value);
    void showPrecisionTestingForm();

private:
    Ui::MainWindow *ui;
    Device *m_device;
    bool m_deviceReady;
    QTimer m_pollingTimer;
};

#endif // MAINWINDOW_H
