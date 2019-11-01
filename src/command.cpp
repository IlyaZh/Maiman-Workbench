#include "command.h"
#include "appsettings.h"

extern AppSettings settings;

Command::Command(QString code, QString unit, double divider, quint8 interval, bool isTemperatureFlag, QObject *parent) : QObject(parent)
{
    this->Code = code;
    this->interval = (interval < MAX_COM_INTERVAL_COUNTER) ? interval : MAX_COM_INTERVAL_COUNTER;
    this->Divider = divider;
    this->isTemperatureFlag = isTemperatureFlag;
    this->unit = unit;
    setRawValue(0);

    setTemperatureUnit(settings.getTemperatureSymbol());
}

double Command::convertCelToFar(double val) {
    return val * 9.0 / 5.0 + 32.0;
}

double Command::convertFarToCel(double val) {
    return (val - 32.0) * 5.0 / 9.0;
}

void Command::setTemperatureUnit(QString unit) {
    temperatureUnit = unit;
}

QString Command::getCode() {
    return Code;
}

double Command::getValue() {
    if(isTemperature() && temperatureUnit == "F") {
        return convertCelToFar(value);
    } else {
        return value;
    }
}

quint16 Command::getRawValue() {
    return static_cast<quint16>(rawValue.toUInt());
}

double Command::getDivider() {
    return Divider;
}

bool Command::isSignedValue() {
    return false;
}

bool Command::isTemperature() {
    return isTemperatureFlag;
}

QString Command::getUnit() {
    if(isTemperature()) {
        return unit+temperatureUnit;
    } else {
        return unit;
    }
}

// SLOTS are declared below

void Command::setRawValue(quint16 _value) {
    if(rawValue.toUInt() != _value) {
        rawValue.setValue(_value);
        this->value = rawValue.toDouble() / Divider;

        emit valueChanged();
    }
}

quint8 Command::getInterval() {
    return interval;
}
