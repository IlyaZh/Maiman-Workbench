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


QString DeviceLimit::getUnit() {
    return limitCode->getUnit();
}

QString DeviceLimit::getLimitCode() {
    return limitCode->getCode();
}

quint16 DeviceLimit::getLimitRaw() {
    return limitCode->getRawValue();
}

double DeviceLimit::getLimitValue() {
    return limitCode->getValue();
}

QString DeviceLimit::getTitle() {
    return  title;
}

double DeviceLimit::getMinValue() {
    if(minCode != nullptr) {
        min = minCode->getValue();
    }
    return min;
}

double DeviceLimit::getMaxValue() {
    if(maxCode != nullptr) {
        max = maxCode->getValue();
    }
    return max;
}

quint16 DeviceLimit::getMinRaw() {
    if(minCode != nullptr) {
        min = minCode->getRawValue();
    }
    return min;
}

quint16 DeviceLimit::getMaxRaw() {
    if(maxCode != nullptr) {
        max = maxCode->getRawValue();
    }
    return max;
}

double DeviceLimit::getDivider() {
    return limitCode->getDivider();
}
