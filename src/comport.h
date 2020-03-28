#ifndef COMPORT_H
#define COMPORT_H

#include "commondefines.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QByteArray>
#include <QTimer>
#include <QQueue>
#include <QStringList>

#include "appsettings.h"
#include "globals.h"

class ComPort : public QObject
{
    Q_OBJECT
public:
    explicit ComPort(AppSettings *appSettings, QObject *parent = nullptr);
    ~ComPort();
    void close();
    bool isOpen();
//    void stopAndDisconnect();

    static const enum DeviceComCommands {
        START_COM = 0x0008,
        STOP_COM = 0x0010,
        INT_MODE_COM = 0x0020,
        EXT_MODE_COM = 0x0040,
        INT_SYNC_MODE_COM = 0x0200,
        EXT_SYNC_MODE_COM = 0x0400,
        BLOCK_ENABLE_COM = 0x1000,
        BLOCK_DISABLE_COM = 0x2000,
        TEMP_BLOCK_DISABLE_COM = 0x4000,
        TEMP_BLOCK_ENABLE_COM = 0x8000
    } DeviceComCommands;

    static const enum DeviceComMasks {
        DEVICE_MASK = 0x01,
        START_MASK = 1 << 1,
        INT_MODE_MASK = 1 << 2,
        INT_SYNC_MASK = 1 << 4,
        IGNORE_THERMAL_BLOCK_MASK = 1 << 6,
        IGNORE_BLOCKS_MASK = 1 << 7
    } DeviceComMasks;


private:
    AppSettings *settings;
    QSerialPort *serialPort;
    QByteArray buffer;
    QString msgToSend;
    QTimer* timer;
    QTimer* errorTimer;
    qint64 sizeOfPackage;
    QSerialPortInfo *serialportinfo;
    bool lastState;
    bool portIsBusy;
    QChar firstSymbol;
    QString lastSentCommand;
    QQueue<QString> queue2send;
    uint stopCommandDelay;
    quint8 checkStopWritten;
    QList<QByteArray> splitCommands;

signals:
    void errorOccuredSignal(QString);
    void receivedDataSignal(QByteArray);
    void portNewState(bool);
    void portTimeroutOccure();
    void dataIsSentSignal();
    void writeToConsoleSignal(QString, Qt::GlobalColor);
//    void portErrorTimeoutOccure();

public slots:
    void needToRead();
    void putIntoQueue(QString str);
    void setPortState(bool);
    void changeBaudRate(int);
    void clearQueue();
    void setStopCommandDelay(uint);
    void setStopBits(int);

private slots:
    void timeOut();
    void errorTimeout();
    void dataIsWritten(qint64 length);
    void errorSlot(QSerialPort::SerialPortError);
    void writeToPort();
    bool startToSendNextCommand();
    void waitForNextCommandSlot();
};

#endif // COMPORT_H
