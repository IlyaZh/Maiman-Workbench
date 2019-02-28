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
extern AppSettings settings;

typedef struct {} commandValue;

class comPort : public QObject
{
    Q_OBJECT
public:
    explicit comPort(QObject *parent = nullptr);
    ~comPort();
    void close();
    bool isOpen();


private:
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
