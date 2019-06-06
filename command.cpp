#include "command.h"

Command::Command(QString code, double divider, quint8 interval, QObject *parent) : QObject(parent)
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
    return value;
}

quint16 Command::getRawValue() {
    return rawValue.toUInt();
}

double Command::getDivider() {
    return Divider;
}

bool Command::isSignedValue() {
    return false;
}

// SLOTS are declared below

void Command::setRawValue(quint16 _value) {
    rawValue.setValue(_value);
    this->value = rawValue.toDouble() / Divider;
}

quint8 Command::getInterval() {
    return interval;
}
