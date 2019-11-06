#include "signedcommand.h"

SignedCommand::SignedCommand(QString code, QString unit, double divider, quint8 interval, bool isTemperatureFlag, QObject* parent) : Command(code, unit, divider, interval, isTemperatureFlag, parent)
{

}

bool SignedCommand::isSignedValue() {
    return true;
}

void SignedCommand::setRawValue(quint16 value) {
    if(RawValue.toUInt() != value) {
        RawValue.setValue(value);
        this->Value = -static_cast<double>((~(static_cast<qint16>(RawValue.toInt())-1))) / Divider;

        emit valueChanged();
    }
}
