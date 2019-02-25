#ifndef COMPORT_H
#define COMPORT_H

#include <QObject>
#include "commondefines.h"
#include <QString>
#include <QStringList>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QByteArray>
#include <QTimer>
#include <QQueue>
#include <QStringList>

#include "globals.h"
extern AppSettings settings;

/*typedef struct {
    QString portName;
    qint32 baudRate;
} comPortSettings_t;*/

typedef struct {} commandValue;

class comPort : public QObject
{
    Q_OBJECT
public:
    explicit comPort(QObject *parent = nullptr);
    ~comPort();
    void close();
    bool isOpen();
//    quint16 getValue(QString);


private:
    QSerialPort *serialPort;
    QByteArray buffer;
    QString msgToSend;
    //int bufferSize;
    //comPortSettings_t comPortSettings;
    QTimer* timer;
    QTimer* errorTimer;
    qint64 sizeOfPackage;
    QSerialPortInfo *serialportinfo;
    bool lastState;
//    bool isTimeout;
    bool portIsBusy;
    QChar firstSymbol;
    QString lastSentCommand;
    QQueue<QString> queue2send;
    //QMap<quint16, quint16> valueTable;
    uint stopCommandDelay;

signals:
    void errorOccuredSignal(QString);
    void receivedDataSignal(QByteArray);
    void portNewState(bool);
    void portTimeroutOccure();
    void dataIsSentSignal();
    void writeToConsoleSignal(QString, Qt::GlobalColor);
    void portErrorTimeoutOccure();

public slots:
    void needToRead();
    void putIntoQueue(QString str);
    void setPortState(bool);
    void changeBaudRate(int);
    void clearQueue();
    void setStopCommandDelay(uint);
    void setStopBits(int);
//    void checkPortForRestart();

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
