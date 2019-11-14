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

void SignedCommand::setRawValue(quint16 inValue) {
    if(rawValue.toUInt() != inValue) {
        rawValue.setValue(inValue);
        this->iValue = static_cast<qint16>(rawValue.toUInt());
        this->value = static_cast<double>(this->iValue) / Divider;

        emit valueChanged();
    }
}
