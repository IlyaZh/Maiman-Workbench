#ifndef PARAMETER_T_H
#define PARAMETER_T_H

#include <QObject>
#include <QString>
#include <QDebug>
#include "commondefines.h"

class Command : public QObject
{
    Q_OBJECT
public:
    explicit Command(QString code, QString unit = "", double divider = 1, quint8 interval = 1, bool isTemperature = false, QObject *parent = nullptr);
    void setTemperatureUnit(QString unit);
    QString getCode();
    virtual double getValue();
    virtual double getConvertedValue();
    quint16 getRawValue();
    quint8 getInterval();
    double getDivider();
    virtual bool isSignedValue();
    bool isTemperature();
    QString getUnit();

    static double convertCelToFar(double val);
    static double convertFarToCel(double val);

signals:
    void valueChanged();

public slots:
    virtual void setRawValue(quint16 value);

protected:
    QString temperatureUnit;
    QString unit;
    QString Code;
    double Divider;
    bool isTemperatureFlag;
    QVariant rawValue;
    double value;
    quint8 interval;
};

#endif // PARAMETER_T_H
