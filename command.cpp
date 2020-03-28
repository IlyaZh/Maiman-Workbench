#include "command.h"
#include "appsettings.h"

extern AppSettings settings;

Command::Command(QString code, QString unit, double divider, quint8 interval, bool isTemperatureFlag, QObject *parent) : QObject(parent)
{
    this->Code = code;
    this->interval = (interval < MAX_COM_INTERVAL_COUNTER) ? interval : MAX_COM_INTERVAL_COUNTER;
    if(this->interval < 1) this->interval = 1;
    this->Divider = divider;
<<<<<<< HEAD
    this->isTemperatureFlag = isTemperatureFlag;
    this->unit = unit;
    rawValue.setValue(-1);
    tick = 0;
=======
    this->IsTemperatureFlag = isTemperatureFlag;
    this->Unit = unit;
    RawValue.setValue(0);
>>>>>>> update

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
    emit valueChanged();
}

QString Command::getTemperatureUnit() {
    return temperatureUnit;
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

int Command::getIValue() {
    if(isTemperature() && temperatureUnit == "F") {
        return qRound(convertCelToFar(value)*getDivider());
    } else {
        return iValue;
    }
}

int Command::getRawValue() {
    return static_cast<quint16>(rawValue.toUInt());
}

double Command::getDivider() {
    return Divider;
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

bool Command::isSignedValue() {
    return false;
}

// SLOTS are declared below

void Command::setRawValue(quint16 iValue) {
//    if(rawValue.toUInt() != iValue) {
        rawValue.setValue(iValue);
        this->iValue = static_cast<quint16>(rawValue.toUInt());
        this->value = rawValue.toDouble() / Divider;

        emit valueChanged();
//    }
}

bool Command::needToRequest() {
    bool result = false;
    if((tick % interval) == 0) {
        result = true;
    }

    tick++;
    if(tick > MAX_COM_INTERVAL_COUNTER) tick = 0;

    return result;
}

void Command::resetInterval() {
    tick = interval;
}
