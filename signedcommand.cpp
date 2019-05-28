#include "signedcommand.h"

SignedCommand::SignedCommand(QString code, double divider, quint8 interval, QObject* parent) : Command(code, divider, interval, parent)
{

}

bool SignedCommand::isSignedValue() {
    return true;
}

double SignedCommand::getConvertedValue() {
    return value;
}

void SignedCommand::setRawValue(quint16 value) {
    rawValue = value;
    this->value = -(~(static_cast<int>(rawValue)-1)) / Divider;
}
