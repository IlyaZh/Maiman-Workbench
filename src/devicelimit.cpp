#include "devicelimit.h"

int DeviceLimit::Count = 0;

DeviceLimit::DeviceLimit(QString title, Command* valueComm, Command* minComm, Command* maxComm, QObject *parent) : QObject(parent)
{
    this->title = title;
    this->valueComm = valueComm;
    this->minComm = minComm;
    this->maxComm = maxComm;
    min = minComm->getValue();
    max = maxComm->getValue();
    Count++;
}

DeviceLimit::DeviceLimit(QString title, Command* valueComm, quint16 minValue, Command* maxComm, QObject *parent) : QObject(parent)
{
    this->title = title;
    this->valueComm = valueComm;
    this->minComm = nullptr;
    this->maxComm = maxComm;
    min = minValue;
    max = maxComm->getValue();
    Count++;
}

DeviceLimit::DeviceLimit(QString title, Command* valueComm, Command* minComm, quint16 maxValue, QObject *parent) : QObject(parent)
{
    this->title = title;
    this->valueComm = valueComm;
    this->minComm = minComm;
    this->maxComm = nullptr;
    min = minComm->getValue();
    max = maxValue;
    Count++;
}

DeviceLimit::DeviceLimit(QString title, Command* valueComm, quint16 minValue, quint16 maxValue, QObject *parent) : QObject(parent)
{
    this->title = title;
    this->valueComm = valueComm;
    this->minComm = nullptr;
    this->maxComm = nullptr;
    min = minValue;
    max = maxValue;
    Count++;
}

DeviceLimit::~DeviceLimit() {
    Count--;
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
