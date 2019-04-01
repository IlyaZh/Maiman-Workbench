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
    return rawValue;
}

double Command::getDivider() {
    return Divider;
}

bool Command::isSignedValue() {
    return false;
}

// SLOTS are declared below

void Command::setRawValue(quint16 value) {
    rawValue = value;
    value = static_cast<double>(rawValue) / Divider;
}

quint8 Command::getInterval() {
    return interval;
}
