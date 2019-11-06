#include "command.h"
#include "appsettings.h"

Command::Command(QString code, QString unit, double divider, quint8 interval, bool isTemperatureFlag, QObject *parent) : QObject(parent)
{
    this->Code = code;
    this->Interval = (interval < MAX_COM_INTERVAL_COUNTER) ? interval : MAX_COM_INTERVAL_COUNTER;
    this->Divider = divider;
    this->IsTemperatureFlag = isTemperatureFlag;
    this->Unit = unit;
    RawValue.setValue(-1);
}

double Command::convertCelToFar(double val) {
    return val * 9.0 / 5.0 + 32.0;
}

double Command::convertFarToCel(double val) {
    return (val - 32.0) * 5.0 / 9.0;
}

void Command::setTemperatureUnit(QString unit) {
    TemperatureUnit = unit;
    emit valueChanged();
}

QString Command::getCode() {
    return Code;
}

double Command::getValue() {
    if(isTemperature() && TemperatureUnit == "F") {
        return convertCelToFar(Value);
    } else {
        return Value;
    }
}

quint16 Command::getRawValue() {
    return static_cast<quint16>(RawValue.toUInt());
}

double Command::getDivider() {
    return Divider;
}

bool Command::isSignedValue() {
    return false;
}

bool Command::isTemperature() {
    return IsTemperatureFlag;
}

QString Command::getUnit() {
    if(isTemperature()) {
        return Unit+TemperatureUnit;
    } else {
        return Unit;
    }
}

// SLOTS are declared below

void Command::setRawValue(quint16 _value) {
    if(RawValue.toUInt() != _value) {
        RawValue.setValue(_value);
        this->Value = RawValue.toDouble() / Divider;

        emit valueChanged();
    }
}

quint8 Command::getInterval() {
    return Interval;
}
