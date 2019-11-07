#include "devicelimit.h"

DeviceLimit::DeviceLimit(QString title, QString unit, QString bottomLimitCode, QString upperLimitCode, QString minCode, QString maxCode, double divider, bool showMin, bool showMax)
{
    this->title = title;
    this->unit = unit;
    this->bottomLimitCode = bottomLimitCode;
    this->upperLimitCode = upperLimitCode;
    this->minCode = minCode;
    this->maxCode = maxCode;
    this->divider = divider;
    this->showMin = showMin;
    this->showMax = showMax;

    upper = bottom = max = min = 0;
}

double DeviceLimit::getDivider() {
    return divider;
}

QString DeviceLimit::getBottomLimitCode() {
    return bottomLimitCode;
}

QString DeviceLimit::getUpperLimitCode() {
    return upperLimitCode;
}

QString DeviceLimit::getMinCode() {
    return minCode;
}

QString DeviceLimit::getMaxCode() {
    return maxCode;
}

QString DeviceLimit::getTitle() {
    return title;
}

QString DeviceLimit::getUnit() {
    return unit;
}

double DeviceLimit::getUpperValue() {
    return upper;
}

double DeviceLimit::getBottomValue() {
    return bottom;
}

double DeviceLimit::getMinValue() {
    return min;
}

double DeviceLimit::getMaxValue() {
    return max;
}

bool DeviceLimit::isShowMax() {
    return showMax;
}

bool DeviceLimit::isShowMin() {
    return showMin;
}

void DeviceLimit::setUpperValue(double value) {
    upper = value;
}

void DeviceLimit::setBottomValue(double value) {
    bottom = value;
}

void DeviceLimit::setMinValue(double value) {
    min = value;
}

void DeviceLimit::setMaxValue(double value) {
    max = value;
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

void DeviceLimit::setShowMax(bool flag) {
    showMax = flag;
}

void DeviceLimit::setShowMin(bool flag) {
    showMin = flag;
}
