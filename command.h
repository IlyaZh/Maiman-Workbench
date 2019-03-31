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
    explicit Command(QString code, int divider = 1, quint8 interval = 1, QObject *parent = nullptr);
    const QString getCode();
    double getConvertedValue();
    quint16 getRawValue();
    quint8 getInterval();
    int getDivider();
    virtual bool isSignedValue();

public slots:
    void setRawValue(quint16 value);

private:
    QString Code;
    int Divider;
    bool isTemperature;
    quint16 pValue;
    quint8 interval;
};

#endif // PARAMETER_T_H
