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
    explicit Command(QString code, double divider = 1, quint8 interval = 1, QObject *parent = nullptr);
    const QString getCode();
    virtual double getConvertedValue();
    quint16 getRawValue();
    quint8 getInterval();
    double getDivider();
    virtual bool isSignedValue();

public slots:
    virtual void setRawValue(quint16 value);

protected:
    QString Code;
    double Divider;
    bool isTemperature;
    QVariant rawValue;
    double value;
    quint8 interval;
};

#endif // PARAMETER_T_H
