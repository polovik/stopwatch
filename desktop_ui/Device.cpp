#include <QDebug>
#include <QSerialPortInfo>
#include "Device.h"
#include "UartPort.h"

namespace NS_Device {
    const quint16 DEVICE_USB_VID = 0x1A86;
    const quint16 DEVICE_USB_PID = 0x7523;
}

Device::Device(QObject *parent) :
    QObject(parent), m_uartPort(NULL)
{
    m_uartPort = new UartPort(CONFIGURATION_KEYBOARD);
    m_uartPort->start();

    connect(m_uartPort, SIGNAL(dataReceived(const QByteArray &)), this, SLOT(parseData(const QByteArray &)));
    connect(m_uartPort, SIGNAL(portOpened()), SIGNAL(SIG_DEVICE_OPENED()));
    connect(m_uartPort, SIGNAL(portClosed()), SIGNAL(SIG_DEVICE_CLOSED()));
    connect(&m_deviceAnswerTimer, SIGNAL(timeout()), SIGNAL(SIG_DEVICE_NOT_ANSWER()));
    m_deviceAnswerTimer.setSingleShot(true);
    m_deviceAnswerTimer.setInterval(1000);
}

Device::~Device()
{
//    closeDevice();
}

void Device::openDevice()
{
    QString portName;

    QList<QSerialPortInfo> portsInfo = QSerialPortInfo::availablePorts();
    foreach (const QSerialPortInfo &portInfo, portsInfo) {
        qDebug() << portInfo.portName() << portInfo.vendorIdentifier() << portInfo.productIdentifier()
                 << portInfo.description() << portInfo.manufacturer() << portInfo.serialNumber();
        if ((portInfo.vendorIdentifier() == NS_Device::DEVICE_USB_VID)
            && (portInfo.productIdentifier() == NS_Device::DEVICE_USB_PID)) {
            portName = portInfo.portName();
            qDebug() << "Device is found on" << portName;
            break;
        }
    }

    if (portName.isEmpty()) {
        qWarning() << "Device is disconnected";
        emit SIG_DEVICE_CLOSED();
        return;
    }

    m_uartPort->open(portName);
}

void Device::closeDevice()
{
    if (m_uartPort == NULL) {
        qWarning() << "Serial port has been already closed";
        return;
    }
    m_uartPort->close();
}


void Device::parseData(const QByteArray &rawData)
{
    //  Add new data in common buffer
    m_readBuffer.append(rawData);

    //  Parse packets while incoming buffer isn't empty
    while (parsePacket()) {
    }
}

/**
 *  Packet from Device have next format:
 *  X.Y.Z\n
 *  X and Y - state of sensors
 *  Z - elapsed time in msec
 */
bool Device::parsePacket()
{
    extern bool g_verboseOutput;

    if (g_verboseOutput) {
        qDebug("parse: %d, 0x%s", m_readBuffer.size(), qPrintable(m_readBuffer.toHex().toUpper()));
    }

    //  Verify prefix of packet
    if (m_readBuffer.size() < 6) {
//        qDebug() << "Packet hasn't been got yet. Expect major 6 bytes. Got:" << m_readBuffer.size() << "bytes";
        return false;
    }

    //  Wait until get full packet
    if (!m_readBuffer.contains('\n')) {
        if (g_verboseOutput) {
            qDebug() << "Packet hasn't been got yet. Expected end of line. got:" << m_readBuffer.size() << "bytes";
        }
        return false;
    }

    //  Extract meaningful data and verify packet's format
    const int end = m_readBuffer.indexOf("\r\n");
    const QString packet = m_readBuffer.left(end);
    m_readBuffer.remove(0, end + 2);
    if (g_verboseOutput) {
        qDebug() << "analyze packet:" << packet;
    }
    const QStringList fields = packet.split(',');
    if (fields.count() != 3) {
        qWarning() << "Packet should have 3 field separated by comma. Got:" << packet;
        return true;
    }
    bool ok1 = false;
    bool ok2 = false;
    bool ok3 = false;
    const int sensor1 = fields.at(0).toInt(&ok1);
    const int sensor2 = fields.at(1).toInt(&ok2);
    const quint64 timeMsec = fields.at(2).toULong(&ok3);
    if (!ok1 || !ok2 || !ok3) {
        qWarning() << "Packet's fields should be numeric. Got:" << packet;
        return true;
    }

    if ((sensor1 == 0) && (sensor2 == 0) && (timeMsec == 0)) {
        qDebug() << "handshake is got";
        emit SIG_HANDSHAKED();
        return true;
    }

    emit sensorState(sensor1, sensor2);
    emit totalTime(timeMsec);
    return true;
}

void Device::requestStatus()
{
    QByteArray packet;
    packet[0] = 'C';
    m_uartPort->sendPacket(packet);
}

void Device::requestReset()
{
    QByteArray packet;
    packet[0] = 'R';
    m_uartPort->sendPacket(packet);
}

