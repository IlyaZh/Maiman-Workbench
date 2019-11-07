#ifndef DEVICELIMIT_H
#define DEVICELIMIT_H

#include <QString>

class DeviceLimit
{
public:
    explicit DeviceLimit(QString title, QString unit = "", QString bottomLimitCode = "", QString upperLimitCode = "", QString minCode = "", QString maxCode = "", double divider = 1, bool showMin = false, bool showMax = false);
    double getDivider();
    QString getBottomLimitCode();
    QString getUpperLimitCode();
    QString getMinCode();
    QString getMaxCode();
    QString getTitle();
    QString getUnit();
    double getUpperValue();
    double getBottomValue();
    double getMinValue();
    double getMaxValue();
    bool isShowMax();
    bool isShowMin();

    void setUpperValue(double);
    void setBottomValue(double);
    void setMinValue(double);
    void setMaxValue(double);
//    void setUpperCode(QString);
//    void setBottomCode(QString);
//    void setMinCode(QString);
//    void setMaxCode(QString);
    void setShowMax(bool);
    void setShowMin(bool);

private:
    double divider;
    double upper, bottom, min, max;
    QString upperLimitCode;
    QString minCode;
    QString bottomLimitCode;
    QString maxCode;
    QString title;
    QString unit;
    bool showMin;
    bool showMax;
};

#endif // DEVICELIMIT_H
