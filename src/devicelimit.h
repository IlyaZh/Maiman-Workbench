#ifndef DEVICELIMIT_H
#define DEVICELIMIT_H

#include <QString>
#include <QObject>
#include "command.h"

class DeviceLimit : public QObject
{
    Q_OBJECT
public:
    explicit DeviceLimit(QString title, Command* valueComm, Command* minComm, Command* maxComm, QObject* parent = nullptr);
    explicit DeviceLimit(QString title, Command* valueComm, quint16 minValue, Command* maxComm, QObject* parent = nullptr);
    explicit DeviceLimit(QString title, Command* valueComm, Command* minComm, quint16 maxValue, QObject* parent = nullptr);
    explicit DeviceLimit(QString title, Command* valueComm, quint16 minValue, quint16 maxValue, QObject* parent = nullptr);
    ~DeviceLimit();
    double getDivider();
    QString getTitle();
    QString getUnit();
    double getMin();
    double getMax();
    double getValue();
    QString getCode();

private:
    double min, max;
    Command* minComm;
    Command* maxComm;
    Command* valueComm;
    QString title;
};

#endif // DEVICELIMIT_H
