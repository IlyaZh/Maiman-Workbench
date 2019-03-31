#include "command.h"

Command::Command(QString code, int divider, quint8 interval, QObject *parent) : QObject(parent)
{
    this->Code = code;
    this->interval = (interval < MAX_COM_INTERVAL_COUNTER) ? interval : MAX_COM_INTERVAL_COUNTER;
    this->Divider = divider;
    setRawValue(0);
}

const QString Command::getCode() {
    return Code;
}

double Command::getConvertedValue() {
    return static_cast<double>(pValue) / Divider;
}

//double Command::getConvertedSignedValue() {
//    qint16 val;
//    val = -(~(pValue-1));
//    return static_cast<double>(val) / Divider;
//}

quint16 Command::getRawValue() {
    return pValue;
}

int Command::getDivider() {
    return Divider;
}

bool Command::isSignedValue() {
    return false;
}

// SLOTS are declared below

void Command::setRawValue(quint16 value) {
    pValue = value;
//    emit changeRawOccured(Code);
}

quint8 Command::getInterval() {
    return interval;
}
