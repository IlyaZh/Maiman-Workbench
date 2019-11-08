#include "signedcommand.h"

SignedCommand::SignedCommand(QString code, QString sUnit, double divider, quint8 cInterval, bool isTemperature, QObject* parent) : Command(code, sUnit, divider, cInterval, isTemperature, parent)
{

}

bool SignedCommand::isSignedValue() {
    return true;
}

double SignedCommand::getConvertedValue() {
    return value;
}

void SignedCommand::setRawValue(quint16 iValue) {
//    if(rawValue.toUInt() != iValue) {
        rawValue.setValue(iValue);
        this->value = -static_cast<double>((~(static_cast<qint16>(rawValue.toInt())-1))) / Divider;
//    }
}
