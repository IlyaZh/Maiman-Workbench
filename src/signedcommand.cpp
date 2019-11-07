#include "signedcommand.h"

SignedCommand::SignedCommand(QString code, QString unit, double divider, quint8 interval, bool isTemperatureFlag, QObject* parent) : Command(code, unit, divider, interval, isTemperatureFlag, parent)
{

}

bool SignedCommand::isSignedValue() {
    return true;
}

double SignedCommand::getConvertedValue() {
    return value;
}

void SignedCommand::setRawValue(quint16 value) {
//    if(rawValue.toUInt() != value) {
        rawValue.setValue(value);
        this->value = -static_cast<double>((~(static_cast<qint16>(rawValue.toInt())-1))) / Divider;
//    }
}
