#ifndef DEVICELIMIT_H
#define DEVICELIMIT_H

#include <QString>
#include "command.h"

class DeviceLimit
{
public:
    explicit DeviceLimit(QString title, Command* code, Command* minCode, Command* maxCode);
    explicit DeviceLimit(QString title, Command* code, double minValue, Command* maxCode);
    explicit DeviceLimit(QString title, Command* code, Command* minCode, double maxValue);
    explicit DeviceLimit(QString title, Command* code, double minValue, double maxValue);
    bool isTemperature();
    QString getTemperatureUnit();
    QString getUnit();
    QString getLimitCode();
    quint16 getLimitRaw();
    double getLimitValue();
    QString getTitle();
    double getMinValue();
    double getMaxValue();
    int getMinRaw();
    int getMaxRaw();
    double getDivider();
    void requestCommand();

private:
    double min, max;
    Command* minCode;
    Command* maxCode;
    Command* limitCode;
    QString title;
};

#endif // DEVICELIMIT_H
