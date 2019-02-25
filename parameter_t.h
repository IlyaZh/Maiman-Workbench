#ifndef PARAMETER_T_H
#define PARAMETER_T_H

#include <QObject>
#include <QString>
#include <QDebug>
#include "commondefines.h"

class parameter_t : public QObject
{
    Q_OBJECT
public:
    explicit parameter_t(QObject *parent = 0);
    const QString getCode();
//    quint8 getAction();
//    int getDivider();
    double getConvertedValue();
    double getConvertedSignedValue();
    quint16 getRawValue();
    quint8 getInterval();
//    bool getTemperatureFlag();

signals:
    void changeOccured(QString code);
    void changeRawOccured(QString code);
public slots:
    void setCode(QString str);
//    void setAction(quint8 value);
    void setDivider(int value);
//    void setConvertedValue(double value);
    void setRawValue(quint16 value);
//    void setTemperatureFlag(bool state);
    void setInterval(quint8 value);

private:
    QString Code;
//    quint8 Action;
    int Divider;
    bool isTemperature;
    quint16 pValue;
    quint8 interval;
};

#endif // PARAMETER_T_H
