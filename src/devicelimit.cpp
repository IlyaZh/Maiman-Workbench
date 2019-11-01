#include "devicelimit.h"

DeviceLimit::DeviceLimit(QString title, Command* valueComm, Command* minComm, Command* maxComm) {
    this->title = title;
    this->valueComm = valueComm;
    this->minComm = minComm;
    this->maxComm = maxComm;
    min = minComm->getValue();
    max = maxComm->getValue();
}

DeviceLimit::DeviceLimit(QString title, Command* valueComm, quint16 minValue, Command* maxComm) {
    this->title = title;
    this->valueComm = valueComm;
    this->minComm = nullptr;
    this->maxComm = maxComm;
    min = minValue;
    max = maxComm->getValue();
}

DeviceLimit::DeviceLimit(QString title, Command* valueComm, Command* minComm, quint16 maxValue) {
    this->title = title;
    this->valueComm = valueComm;
    this->minComm = minComm;
    this->maxComm = nullptr;
    min = minComm->getValue();
    max = maxValue;
}

DeviceLimit::DeviceLimit(QString title, Command* valueComm, quint16 minValue, quint16 maxValue) {
    this->title = title;
    this->valueComm = valueComm;
    this->minComm = nullptr;
    this->maxComm = nullptr;
    min = minValue;
    max = maxValue;
}

DeviceLimit::~DeviceLimit() {
    valueComm->disconnect();
    if(minComm != nullptr) minComm->disconnect();
    if(maxComm != nullptr) maxComm->disconnect();
}

double DeviceLimit::getDivider() {
    return valueComm->getDivider();
}

QString DeviceLimit::getTitle() {
    return title;
}

QString DeviceLimit::getUnit() {
    return valueComm->getUnit();
}

double DeviceLimit::getMin() {
    if(minComm == nullptr) {
        return min;
    } else {
        return minComm->getValue();
    }
}

double DeviceLimit::getMax() {
    if(maxComm == nullptr) {
        return max;
    } else {
        return maxComm->getValue();
    }
}

double DeviceLimit::getValue() {
    return valueComm->getValue();
}

QString DeviceLimit::getCode() {
    return  valueComm->getCode();
}


//void DeviceLimit::setUpperCode(QString str) {
//    upperLimitCode = str;
//}

//void DeviceLimit::setBottomCode(QString str) {
//    bottomLimitCode = str;
//}

//void DeviceLimit::setMinCode(QString str) {
//    minCode = str;
//}

//void DeviceLimit::setMaxCode(QString str) {
//    maxCode = str;
//}

//void DeviceLimit::setShowMax(bool flag) {
//    showMax = flag;
//}

//void DeviceLimit::setShowMin(bool flag) {
//    showMin = flag;
//}
