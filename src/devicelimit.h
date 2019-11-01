#ifndef DEVICELIMIT_H
#define DEVICELIMIT_H

#include <QString>
#include "command.h"

class DeviceLimit
{
public:
    explicit DeviceLimit(QString title, Command* valueComm, Command* minComm, Command* maxComm);
    explicit DeviceLimit(QString title, Command* valueComm, quint16 minValue, Command* maxComm);
    explicit DeviceLimit(QString title, Command* valueComm, Command* minComm, quint16 maxValue);
    explicit DeviceLimit(QString title, Command* valueComm, quint16 minValue, quint16 maxValue);
    ~DeviceLimit();
    double getDivider();
    QString getTitle();
    QString getUnit();
    double getMin();
    double getMax();
    double getValue();
    QString getCode();

//    void setUpperCode(QString);
//    void setBottomCode(QString);
//    void setMinCode(QString);
//    void setMaxCode(QString);

private:
    double min, max;
    Command* minComm;
    Command* maxComm;
    Command* valueComm;
    QString title;
};

#endif // DEVICELIMIT_H
