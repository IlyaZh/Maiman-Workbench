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
    static int Count;
    explicit Command(QString code, QString unit = "", double divider = 1, quint8 interval = 1, bool isTemperature = false, QObject *parent = nullptr);
    ~Command();
    void setTemperatureUnit(QString unit);
    QString getCode();
    virtual double getValue();
    quint16 getRawValue();
    quint8 getInterval();
    double getDivider();
    virtual bool isSignedValue();
    bool isTemperature();
    QString getUnit();

    static double convertCelToFar(double val);
    static double convertFarToCel(double val);

public slots:
    virtual void setRawValue(quint16 value);

signals:
//    void valueChanged();

protected:
    QString TemperatureUnit;
    QString Unit;
    QString Code;
    double Divider;
    bool IsTemperatureFlag;
    QVariant RawValue;
    double Value;
    quint8 Interval;
};

#endif // PARAMETER_T_H
