#include "parameter_t.h"

parameter_t::parameter_t(QObject *parent) : QObject(parent)
{
//    setAction(0);
    setCode("");
    setDivider(1);
    setRawValue(0);
    setInterval(1);
}

const QString parameter_t::getCode() {
    return Code;
}

//quint8 parameter_t::getAction() {
//    return Action;
//}

//int parameter_t::getDivider() {
//    return Divider;
//}

double parameter_t::getConvertedValue() {
    return ((double) pValue) / Divider;
}

double parameter_t::getConvertedSignedValue() {
    qint16 val;
    val = -(~(pValue-1));
    return ((double) val) / Divider;
}

quint16 parameter_t::getRawValue() {
    return pValue;
}

//bool parameter_t::getTemperatureFlag() {
//    return isTemperature;
//}


// SLOTS are declared below

void parameter_t::setCode(QString str) {
    if(!str.isEmpty()) Code = str;
}

//void parameter_t::setAction(quint8 value) {
//    Action = value;
//}

void parameter_t::setDivider(int value) {
    Divider = value;
    emit changeOccured(Code);
}

/*void parameter_t::setConvertedValue(double value) {
    pValue = qRound(value*Divider);
    //emit changeConvertedOccured(Code);
}*/

void parameter_t::setRawValue(quint16 value) {
    pValue = value;
    emit changeRawOccured(Code);
}

//void parameter_t::setTemperatureFlag(bool state) {
//    isTemperature = state;
//}

quint8 parameter_t::getInterval() {
    return interval;
}

void parameter_t::setInterval(quint8 value) {
    if (value > MAX_COM_INTERVAL_COUNTER) value = MAX_COM_INTERVAL_COUNTER;
    interval = value;
}
