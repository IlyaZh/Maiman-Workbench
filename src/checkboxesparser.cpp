#include "checkboxesparser.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QTextStream>

checkboxesParser::checkboxesParser(QString fileName, QObject *parent) : QObject(parent), file(nullptr)
{
    setFile(fileName);
}

checkboxesParser::~checkboxesParser() {
    if(file != nullptr) file->deleteLater();
}


void checkboxesParser::setFile(QString file) {
    fileName = file;
}

void checkboxesParser::loadData(quint16 deviceId) {}

bool checkboxesParser::hasDataForDevice(quint16 devId) {}
