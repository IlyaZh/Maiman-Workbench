#include "signedcommand.h"

SignedCommand::SignedCommand(QString code, QString sUnit, double divider, quint8 cInterval, bool isTemperature, QObject* parent) : Command(code, sUnit, divider, cInterval, isTemperature, parent)
{

}

bool SignedCommand::isSignedValue() {
    return true;
}

//double SignedCommand::getValue() {
//    return value;
//}

int SignedCommand::getRawValue() {
    return static_cast<qint16>(rawValue.toInt());
}

void SignedCommand::setRawValue(quint16 inValue) {
    if(rawValue.toUInt() != inValue) {
        rawValue.setValue(inValue);
        this->iValue = static_cast<qint16>(rawValue.toUInt());
        this->value = static_cast<double>(this->iValue) / Divider;

        emit valueChanged();
    }
}
