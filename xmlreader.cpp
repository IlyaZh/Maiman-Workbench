#include "xmlreader.h"

xmlReader::xmlReader(QObject *parent) : QObject(parent)
{
    file = nullptr;
}

xmlReader::~xmlReader() {
//    if(file) delete file;
}



void xmlReader::setDeviceOptions(device_t &device, quint16 dev_id) {
    dev = &device;
    dev->hasLaser = false;
    dev->hasTEC = false;
    dev->startTECwithLaser = false;
    deviceId = dev_id;
    dev->stopCommandDelay = STOP_COMMAND_DELAY_DEFAULT;
    dev->minCommDelay = COM_COMMAND_MIN_SEND_DELAY;
    dev->maxCommDelay = COM_COMMAND_MAX_SEND_DELAY;
//    currDevIndex = 0;
}

void xmlReader::setList(QList<availableDev_t>& listPtr) {
    availableListPtr = &listPtr;
}

void xmlReader::setBaudsList(QList<uint> &baudList) {
    bauds = &baudList;
}

void xmlReader::setConfigFile(QString file) {
    filePath = file;
}

bool xmlReader::readFile() {
    file = new QFile();
    file->setFileName(filePath);
    if(!file->open(QIODevice::ReadOnly)) {
        emit errorHandler("CONFIG: " + file->errorString());
        return false;
    }

    return true;
}

void xmlReader::readProgramConfig() {
    if(!readFile()) return;

    availableListPtr->clear();
    bauds->clear();
    xml.setDevice(file);

    QXmlStreamAttributes attrib = xml.attributes();
    QStringRef xname = xml.name();

    while(!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        xname = xml.name();
        if(token == QXmlStreamReader::StartElement) {
            if(xname == "CIDD") {
                attrib = xml.attributes();
                if(attrib.hasAttribute("id")) {
                    availableDev_t devStruct;
                    devStruct.id = attrib.value("id").toUInt(nullptr, 16);
                    devStruct.name = xml.readElementText();
                    availableListPtr->append(devStruct);
                }

            } else if(xname == "BaudRate") {
                attrib = xml.attributes();
                if(attrib.hasAttribute("value")) {
                    uint value = attrib.value("value").toUInt();
                    bauds->append(value);
                }
            }
        } else if(token == QXmlStreamReader::EndElement) {
            // Завершаем цикл чтения по прошествию соответствующего закрывающего тэга
            if(xname == "CommonIDDevices") break;
        }
    }

    if(xml.hasError()) emit errorHandler("Program CONFIG: " + xml.errorString());
    emit endOfLoadingProgramConfig(!xml.hasError());

    file->close();
}

void xmlReader::startLoading() {
    if(!readFile()) return;
    bool isDeviceFound = false;
//    currDeviceFound = false;

    xml.setDevice(file);


    while(!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if(token == QXmlStreamReader::StartElement) {
            QStringRef xname = xml.name();
            if (xname == "Device") {
                if(isDeviceFound) {
                    emit endOfLoadingConfig(true);
                    return;
                }
                isDeviceFound = parseDevice();
            }
            if(!isDeviceFound) continue;


            if (xname == "content") {
                parseContent();
//                currDevIndex++;
            } else if (xname == "limit") {
                parseLimit();
            } else if (xname == "calibrate") {
                parseCalibration();
            } else if (xname == "command") {
                parseCommand();
            } else if (xname == "paramControls") {
                parseControls();
            } else if (xname == "led") {
                parseLed();
            } else if (xname == "ledMask") {
                parseLedMask();
            } else if (xname == "specialParams") {
                parseSpecialParam();
            } else if (xname == "button") {
                parseButtons();
            } else {
                // незивестный тэг.
                // игнорируем
            }
        } else if(token == QXmlStreamReader::EndElement) {
            //if(xml.name() == "Device") break;
        }
    }

    // Обработчик команд для неизвестных устройств
    // (отсутствует)

    if(xml.hasError()) {
        emit errorHandler("CONFIG: " + xml.errorString());
    } else {
        emit endOfLoadingConfig(isDeviceFound);
    }

    file->close();
}

bool xmlReader::parseDevice() {
    QXmlStreamAttributes attrib = xml.attributes();
    if(attrib.hasAttribute("id")) {
        quint16 currID = attrib.value("id").toUInt(nullptr, 16);
        if(currID != deviceId) return false;
        dev->deviceID = currID;
        if (attrib.hasAttribute("name")) dev->devName = attrib.value("name").toString();
        if(attrib.hasAttribute("minStopCommandDelayMs")) {
            dev->minCommDelay = attrib.value("minStopCommandDelayMs").toUInt();
        }

        if(attrib.hasAttribute("maxStopCommandDelayMs")) {
            dev->maxCommDelay = attrib.value("maxStopCommandDelayMs").toUInt();
        }

        if(attrib.hasAttribute("stopCommandDelayMs")) {
            dev->stopCommandDelay = attrib.value("stopCommandDelayMs").toUInt();
        }

        return true;
    }
    return false;
}

void xmlReader::parseContent() {
    while(xml.readNextStartElement()) {
        if(xml.name() == "name") {
//            availableDevices[currDevIndex].devName = xml.readElementText();
            dev->devName = xml.readElementText();
        } else if(xml.name() == "image") {
//            availableDevices[currDevIndex].image = xml.readElementText();
            dev->image = xml.readElementText();
        } else if(xml.name() == "description") {
//            availableDevices[currDevIndex].description = xml.readElementText();
            dev->description = xml.readElementText();
        } else if(xml.name() == "link") {
//            availableDevices[currDevIndex].link = xml.readElementText();
            dev->link = xml.readElementText();
        } else {
            xml.skipCurrentElement();
        }
    }
}

void xmlReader::parseCommand() {

    parameter_t *tmpPar = new parameter_t();

    QXmlStreamAttributes attrib = xml.attributes();

    if(attrib.hasAttribute("code")) {
        tmpPar->setCode(attrib.value("code").toString());
    }

//    if(attrib.hasAttribute("action")) {
//        QString tmpString = attrib.value("action").toString();
//        if(tmpString == "R") tmpPar->setAction(READ);
//        if(tmpString == "W") tmpPar->setAction(WRITE);
//        if(tmpString == "RW") tmpPar->setAction(READ+WRITE);
//    }

    if(attrib.hasAttribute("divider")) {
        tmpPar->setDivider(attrib.value("divider").toDouble());
    }

    if(attrib.hasAttribute("defaultValue")) {
        tmpPar->setRawValue(attrib.value("defaultValue").toUInt());
    }

    if(attrib.hasAttribute("interval")) {
        tmpPar->setInterval(attrib.value("interval").toUInt());
    }

//    dev->commands.insert(tmpPar.getCode(), &tmpPar);
    dev->commands.append(tmpPar);

}

void xmlReader::parseControls() {
//    if(!currDeviceFound) return;
    while(xml.readNextStartElement()) {
        if(xml.name() == "param")
            parseParam();
    }
}

//void xmlReader::parseLeds() {
//    while(xml.readNextStartElement()) {
//        if(xml.name() == "led")
//            parseLed();
//        else if (xml.name() == "ledMask")
//            parseLedMask();
//    }
//}

void xmlReader::parseLed() {
    leds_t led;
    QXmlStreamAttributes attrib = xml.attributes();
    if(attrib.hasAttribute("label")) {
        led.label = attrib.value("label").toString();
    }
    dev->leds.append(led);
//    lastLed = dev->leds.length()-1;
}

void xmlReader::parseLedMask() {
    if(dev->leds.isEmpty()) return;

    ledMask_t ledMask;
    ledMask.message.clear();
    ledMask.activeColor = LEDS_DEFAULT_ACTIVE_COLOR;
    ledMask.defaultColor = LEDS_DEFAULT_COLOR;
    QXmlStreamAttributes attrib = xml.attributes();
    if(attrib.hasAttribute("command")) {
        ledMask.command = attrib.value("command").toString();
    }
    if(attrib.hasAttribute("mask")) {
        ledMask.mask = attrib.value("mask").toUInt(nullptr, 16);
    }
    if(attrib.hasAttribute("maskColor")) {
        ledMask.activeColor.setNamedColor(attrib.value("maskColor").toString());
    }
    if(attrib.hasAttribute("defaultColor")) {
        ledMask.defaultColor.setNamedColor(attrib.value("defaultColor").toString());
    }

    QString maskMsg = xml.readElementText();
    if(!maskMsg.isEmpty())
        ledMask.message = maskMsg;

    dev->leds.last().masks.append(ledMask);
}

void xmlReader::parseParam() {
    QXmlStreamAttributes attrib = xml.attributes();

    ParameterForm* PFptr = new ParameterForm();
//    parameterTemp_t paramTemp;

    if(attrib.hasAttribute("isTemperature")) {
        PFptr->setIsTemperatureFlag((attrib.value("isTemperature").toUInt() == 1) ? true : false);
//        paramTemp.isTemperatureFlag = (attrib.value("isTemperature").toUInt() == 1) ? true : false;
    } else {
        PFptr->setIsTemperatureFlag(false);
    }

    if(attrib.hasAttribute("unit")) {
        QString tmpStr = attrib.value("unit").toString();
        tmpStr.replace("(deg)", QString::fromRawData(new QChar('\260'), 1));
        PFptr->setUnit(tmpStr);
//        paramTemp.unit = tmpStr;
    }

    if(attrib.hasAttribute("min")) {
        PFptr->setMinComm(attrib.value("min").toString());
//        paramTemp.minComm = attrib.value("min").toString();
    }

    if(attrib.hasAttribute("max")) {
        PFptr->setMaxComm(attrib.value("max").toString());
//        paramTemp.maxComm = attrib.value("max").toString();
    }

    if(attrib.hasAttribute("value")) {
        PFptr->setValueComm(attrib.value("value").toString());
//        paramTemp.valueComm = attrib.value("value").toString();
    }

    if(attrib.hasAttribute("real")) {
        PFptr->setRealComm(attrib.value("real").toString());
//        paramTemp.realComm = attrib.value("real").toString();
    }

    if(attrib.hasAttribute("divider")) {
        PFptr->setDivider(attrib.value("divider").toDouble());
//        paramTemp.divider = attrib.value("divider").toDouble();
    }

    PFptr->setTitle(xml.readElementText());
//    paramTemp.title = xml.readElementText();
    dev->paramWidgets.append(PFptr);
//    tempParams.append(paramTemp);
}

void xmlReader::parseLimit() {
//    if(!currDeviceFound) return;
    QXmlStreamAttributes attrib = xml.attributes();
    limits_t tmpStruct;
    tmpStruct.absMaxCode.clear();
    tmpStruct.absMinCode.clear();
    tmpStruct.maxCode.clear();
    tmpStruct.minCode.clear();
    tmpStruct.title.clear();
    tmpStruct.divider = 1;
//    tmpStruct.absMinValue = -100000;
//    tmpStruct.absMaxValue = -100000;
//    tmpStruct.absMinValue = -100000;
//    tmpStruct.absMaxValue = -100000;

//    if(attrib.hasAttribute("absMinValue")) {
//        tmpStruct.absMinValue = attrib.value("absMinValue").toDouble();
//    }

//    if(attrib.hasAttribute("absMaxValue")) {
//        tmpStruct.absMaxValue = attrib.value("absMaxValue").toDouble();
//    }

//    if(attrib.hasAttribute("maxValue")) {
//        tmpStruct.maxValue = attrib.value("maxValue").toDouble();
//    }

//    if(attrib.hasAttribute("minValue")) {
//        tmpStruct.minValue = attrib.value("minValue").toDouble();
//    }

    if(attrib.hasAttribute("absMinCode")) {
        tmpStruct.absMinCode = attrib.value("absMinCode").toString();
    }

    if(attrib.hasAttribute("minCode")) {
        tmpStruct.minCode = attrib.value("minCode").toString();
    }

    if(attrib.hasAttribute("absMaxCode")) {
        tmpStruct.absMaxCode = attrib.value("absMaxCode").toString();
    }

    if(attrib.hasAttribute("maxCode")) {
        tmpStruct.maxCode = attrib.value("maxCode").toString();
    }

    if(attrib.hasAttribute("divider")) {
        tmpStruct.divider = attrib.value("divider").toDouble();
    }

    if(attrib.hasAttribute("unit")) {
        tmpStruct.unit = attrib.value("unit").toString();
    }

    tmpStruct.title = xml.readElementText();

    dev->limits.append(tmpStruct);
}

void xmlReader::parseCalibration() {
    QXmlStreamAttributes attrib = xml.attributes();
    calibration_t tmpStruct;
    tmpStruct.code = "";
    tmpStruct.title = "No name";
    tmpStruct.min = 0;
    tmpStruct.max = 10;
    tmpStruct.divider = 1;

    if(attrib.hasAttribute("code")) {
        tmpStruct.code = attrib.value("code").toString();
    }

    if(attrib.hasAttribute("min")) {
        tmpStruct.min = attrib.value("min").toInt();
    }

    if(attrib.hasAttribute("max")) {
        tmpStruct.max = attrib.value("max").toInt();
    }

    if(attrib.hasAttribute("divider")) {
        tmpStruct.divider = attrib.value("divider").toDouble();
    }

    tmpStruct.title = xml.readElementText();

    dev->calibrate.append(tmpStruct);

}

void xmlReader::parseButtons() {
    doubleMaskCommand_t tmp;
    QXmlStreamAttributes attrib = xml.attributes();
    QString tmpName = "noname";
//    QPushButton* btn = new QPushButton();
//    tmp.btnPtr = btn;
    tmp.mask = 0;

//    btn->setStyleSheet("QPushButton {\n	color: #000;\n	border: 1px solid rgb(31,31,31);\n	border-radius: 4px;\n	padding: 3px 20px;\n	background-color: rgb(189, 1, 2);\n}\n\nQPushButton::checked {\n	background-color: rgb(0, 102, 52);\n}");

    if(attrib.hasAttribute("name")) {
        tmpName = attrib.value("name").toString();
//        btn->setText(tmpName);
        if(tmpName == "laser") {
            dev->hasLaser = true;
        } else if (tmpName == "tec") {
            dev->hasTEC = true;
        }
    }

    if(attrib.hasAttribute("code")) {
        tmp.code = attrib.value("code").toString();
    }
    if(attrib.hasAttribute("mask")) {
        tmp.mask = attrib.value("mask").toUInt(nullptr, 16);
    }

    if(attrib.hasAttribute("onCommand")) {
        tmp.onCommand = attrib.value("onCommand").toString();
    }

    if(attrib.hasAttribute("offCommand")) {
        tmp.offCommand = attrib.value("offCommand").toString();
    }

    dev->stateButtons.insert(tmpName, tmp);
}

/*void xmlReader::parseInWindowItems() {
    while(xml.readNext() != QXmlStreamReader::EndElement) {
        if(xml.name() == "param") {
            QXmlStreamAttributes attrib = xml.attributes();
            if(attrib.hasAttribute("linkCode")) {
                dev->showInWindowParameters.insert(attrib.value("linkCode").toString(), xml.readElementText());
            }
        }
    }
}*/

void xmlReader::parseSpecialParam() {
//    if(!currDeviceFound) return;
    while(xml.readNext() != QXmlStreamReader::EndElement) {
        if(xml.name() == "specParam") {
            specParams_t tmp;
            QXmlStreamAttributes attrib = xml.attributes();

            if(attrib.hasAttribute("code")) {
                tmp.code = attrib.value("code").toString();
            }
            if(attrib.hasAttribute("offCommand")) {
                tmp.offCommand = attrib.value("offCommand").toString();
            }
            if(attrib.hasAttribute("onCommand")) {
                tmp.onCommand = attrib.value("onCommand").toString();
            }
            if(attrib.hasAttribute("mask")) {
                tmp.mask = attrib.value("mask").toUInt(nullptr, 16);
            } else {
                tmp.mask = 0;
            }

            if(attrib.hasAttribute("id")) {
                tmp.id = attrib.value("id").toUInt(nullptr, 16);
            }


            QCheckBox *cb = new QCheckBox();
            QString label = xml.readElementText();
            cb->setText(label);
            tmp.label = label;
            cb->setStyleSheet("border: none;");
            tmp.cbPtr = cb;
            dev->specialParameters.append(tmp);
        }
    }
}

//QVector<quint16>& xmlReader::getavailableDevices() {
        //return availableDevices;
//}
