#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QTimer>
#include <QMap>
#include <QPair>

class UartPort;

class Device : public QObject
{
    Q_OBJECT
public:
    explicit Device(QObject *parent = 0);
    ~Device();
    bool init();
    void close();

public slots:
    void openDevice();
    void closeDevice();
    void requestStatus();
    void requestReset();

signals:
    void SIG_DEVICE_OPENED();
    void SIG_DEVICE_CLOSED();
    void SIG_HANDSHAKED();
    void SIG_DEVICE_NOT_ANSWER();
    void SIG_DEVICE_STATUS();
    void sensorState(int sensor1, int sensor2);
    void totalTime(quint64 msec);

private slots:
    void parseData(const QByteArray &rawData);

private:
    bool parsePacket();

    QByteArray m_readBuffer;
    UartPort* m_uartPort;
    QTimer m_deviceAnswerTimer;
};

#endif // DEVICE_H
