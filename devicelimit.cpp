#include "devicelimit.h"
#include <QDebug>

DeviceLimit::DeviceLimit(QString title, Command* code, Command* minCode, Command* maxCode) {
    this->title = title;
    limitCode = code;
    this->minCode = minCode;
    this->maxCode = maxCode;

    if(limitCode == nullptr)
        qDebug() << "DeviceLimit" << "limitCode pointer is NULL";
    if(minCode == nullptr)
         qDebug() << "DeviceLimit" << "minCode pointer is NULL";
    if(maxCode == nullptr)
         qDebug() << "DeviceLimit" << "maxCode pointer is NULL";
}

DeviceLimit::DeviceLimit(QString title, Command* code, double minValue, Command* maxCode) {
    this->title = title;
    limitCode = code;
    this->minCode = nullptr;
    this->maxCode = maxCode;
    min = minValue;

    if(limitCode == nullptr)
        qDebug() << "DeviceLimit" << "limitCode pointer is NULL";
    if(maxCode == nullptr)
        qDebug() << "DeviceLimit" << "maxCode pointer is NULL";
}

DeviceLimit::DeviceLimit(QString title, Command* code, Command* minCode, double maxValue) {
    this->title = title;
    limitCode = code;
    this->minCode = minCode;
    this->maxCode = nullptr;
    max = maxValue;

    if(limitCode == nullptr)
        qDebug() << "DeviceLimit" << "limitCode pointer is NULL";
    if(minCode == nullptr)
        qDebug() << "DeviceLimit" << "minCode pointer is NULL";
}

DeviceLimit::DeviceLimit(QString title, Command* code, double minValue, double maxValue) {
    this->title = title;
    limitCode = code;
    minCode = nullptr;
    maxCode = nullptr;
    min = minValue;
    max = maxValue;

    if(limitCode == nullptr)
        qDebug() << "DeviceLimit" << "limitCode pointer is NULL";
}

bool DeviceLimit::isTemperature() {
    return limitCode->isTemperature();
}

QString DeviceLimit::getTemperatureUnit() {
    if(isTemperature())
        return limitCode->getTemperatureUnit();
    else
        return "";
}

QString DeviceLimit::getUnit() {
    return limitCode->getUnit();
}

QString DeviceLimit::getLimitCode() {
    return limitCode->getCode();
}

quint16 DeviceLimit::getLimitRaw() {
    return qRound(limitCode->getValue()*limitCode->getDivider());
}

double DeviceLimit::getLimitValue() {
    double res = limitCode->getValue();
    if(abs(res) < 0.0001) res = 0;
    return res;
}

QString DeviceLimit::getTitle() {
    return  title;
}

double DeviceLimit::getMinValue() {
    double res = min;
    if(minCode == nullptr && limitCode->isTemperature() && limitCode->getTemperatureUnit() == "F") {
        res = Command::convertCelToFar(min);
    } else {
        if(minCode != nullptr) {
            res = min = minCode->getValue();
        }
    }
    if(abs(res) < 0.0001) res = 0;
    return res;
}

double DeviceLimit::getMaxValue() {
    double res = max;
    if(maxCode == nullptr && limitCode->isTemperature() && limitCode->getTemperatureUnit() == "F") {
        res = Command::convertCelToFar(max);
    } else {
        if(maxCode != nullptr) {
            res = max = maxCode->getValue();
        }
    }
    if(abs(res) < 0.0001) res = 0;
    return res;
}

int DeviceLimit::getMinRaw() {
    return qRound(getMinValue()*limitCode->getDivider());
}

int DeviceLimit::getMaxRaw() {
    return qRound(getMaxValue()*limitCode->getDivider());
}

double DeviceLimit::getDivider() {
    return limitCode->getDivider();
}

void DeviceLimit::requestCommand() {
    limitCode->resetInterval();
}
