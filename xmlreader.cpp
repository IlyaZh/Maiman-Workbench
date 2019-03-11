#include "xmlreader.h"
#include "command.h"

xmlReader::xmlReader(QObject *parent) : QObject(parent)
{
    file = nullptr;
}

void xmlReader::setDeviceOptions(device_t &device, quint16 dev_id) {
    this->device = &device;
    device.hasLaser = false;
    device.hasTEC = false;
    device.startTECwithLaser = false;
    deviceId = dev_id;
    device.stopCommandDelay = STOP_COMMAND_DELAY_DEFAULT;
    device.minCommDelay = COM_COMMAND_MIN_SEND_DELAY;
    device.maxCommDelay = COM_COMMAND_MAX_SEND_DELAY;
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

            }
            else if(xname == "BaudRate") {
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
    file->deleteLater();
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
//                if(isDeviceFound) break;
                isDeviceFound = parseDevice();
                if(!isDeviceFound) xml.skipCurrentElement();
            }
//            if(!isDeviceFound) continue;

            else if (xname == "content") {
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
            } else if (xname == "specParam") {
                parseSpecialParam();
            } else if (xname == "button") {
                parseButtons();
            } else {
//                xml.skipCurrentElement();
            }
        } else if(token == QXmlStreamReader::EndElement) {
            if(xml.name() == "Device" && isDeviceFound) {
                break;
            }
        }
    }

    // Обработчик команд для неизвестных устройств
    // (отсутствует)

    if(xml.hasError()) {
        emit errorHandler("Error in config at line (" + QString::number(xml.lineNumber()) + "): " + xml.errorString());
    } else {
        emit endOfLoadingConfig(isDeviceFound);
    }

    file->close();
    file->deleteLater();
}

bool xmlReader::parseDevice() {
    QXmlStreamAttributes attrib = xml.attributes();
    if(attrib.hasAttribute("id")) {
        quint16 currID = attrib.value("id").toUInt(nullptr, 16);
        if(currID != deviceId) return false;
        device->deviceID = currID;
        if (attrib.hasAttribute("name")) device->devName = attrib.value("name").toString();
        if(attrib.hasAttribute("minStopCommandDelayMs")) {
            device->minCommDelay = attrib.value("minStopCommandDelayMs").toUInt();
        }

        if(attrib.hasAttribute("maxStopCommandDelayMs")) {
            device->maxCommDelay = attrib.value("maxStopCommandDelayMs").toUInt();
        }

        if(attrib.hasAttribute("stopCommandDelayMs")) {
            device->stopCommandDelay = attrib.value("stopCommandDelayMs").toUInt();
        }

        return true;
    }
    return false;
}

void xmlReader::parseContent() {
    while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "content")) {
        if(xml.name() == "name") {
            device->devName = xml.readElementText();
        } else if(xml.name() == "image") {
            device->image = xml.readElementText();
        } else if(xml.name() == "description") {
            device->description = xml.readElementText();
        } else if(xml.name() == "link") {
            device->link = xml.readElementText();
        }
        xml.readNext();
    }
}

void xmlReader::parseCommand() {
    QXmlStreamAttributes attrib = xml.attributes();

    QString code = (attrib.hasAttribute("code")) ? attrib.value("code").toString() : "";

    double divider = (attrib.hasAttribute("divider")) ? attrib.value("divider").toDouble() : 1;

    quint8 interval = (attrib.hasAttribute("interval")) ? attrib.value("interval").toUInt() : 1;

    device->commands.append(new Command(code, divider, interval));
}

void xmlReader::parseControls() {
    while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "paramControls")) {
        if(xml.name() == "param")
            parseParam();
        xml.readNext();
    }
}

void xmlReader::parseLed() {
    leds_t led;
    QXmlStreamAttributes attrib = xml.attributes();
    if(attrib.hasAttribute("label")) {
        led.label = attrib.value("label").toString();
    }
    device->leds.append(led);
//    lastLed = dev->leds.length()-1;
}

void xmlReader::parseLedMask() {
    if(device->leds.isEmpty()) return;

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

    device->leds.last().masks.append(ledMask);
}

void xmlReader::parseParam() {
    QXmlStreamAttributes attrib = xml.attributes();

    ParameterController* parameterController = new ParameterController();

    if(attrib.hasAttribute("isTemperature")) {
        parameterController->setIsTemperatureFlag((attrib.value("isTemperature").toUInt() == 1) ? true : false);
    } else {
        parameterController->setIsTemperatureFlag(false);
    }

    if(attrib.hasAttribute("unit")) {
        QString tmpStr = attrib.value("unit").toString();
        tmpStr.replace("(deg)", QString::fromRawData(new QChar('\260'), 1));
        parameterController->setUnit(tmpStr);
//        paramTemp.unit = tmpStr;
    }

    if(attrib.hasAttribute("min")) {
        parameterController->setMinComm(attrib.value("min").toString());
//        paramTemp.minComm = attrib.value("min").toString();
    }

    if(attrib.hasAttribute("max")) {
        parameterController->setMaxComm(attrib.value("max").toString());
//        paramTemp.maxComm = attrib.value("max").toString();
    }

    if(attrib.hasAttribute("value")) {
        parameterController->setValueComm(attrib.value("value").toString());
//        paramTemp.valueComm = attrib.value("value").toString();
    }

    if(attrib.hasAttribute("real")) {
        parameterController->setRealComm(attrib.value("real").toString());
//        paramTemp.realComm = attrib.value("real").toString();
    }

    if(attrib.hasAttribute("divider")) {
        parameterController->setDivider(attrib.value("divider").toDouble());
//        paramTemp.divider = attrib.value("divider").toDouble();
    }

    parameterController->setTitle(xml.readElementText());
//    paramTemp.title = xml.readElementText();
    device->paramWidgets.append(parameterController);
//    tempParams.append(paramTemp);
}

void xmlReader::parseLimit() {
//    if(!currDeviceFound) return;
    QXmlStreamAttributes attrib = xml.attributes();
    // If there is no both codes (min and max) then it is a wrong configuration of limit
    if(!(attrib.hasAttribute("minCode") || attrib.hasAttribute("maxCode"))) return;

    bool showMin = false;
    bool showMax = false;

    if(attrib.hasAttribute("show")) {
        QString showValue = attrib.value("show").toString();
        if(showValue.compare("min", Qt::CaseInsensitive) == 0) {
            showMin = true;
        } else if(showValue.compare("max", Qt::CaseInsensitive) == 0) {
            showMax = true;
        } else if(showValue.compare("both", Qt::CaseInsensitive) == 0) {
            showMax = showMin = true;
        }
    }

    DeviceLimit* devLimit = new DeviceLimit(xml.readElementText(),
                                 attrib.hasAttribute("unit") ? attrib.value("unit").toString() : "",
                                 attrib.hasAttribute("bottomCode") ? attrib.value("bottomCode").toString() : "",
                                 attrib.hasAttribute("upperCode") ? attrib.value("upperCode").toString() : "",
                                 attrib.hasAttribute("minCode") ? attrib.value("minCode").toString() : "",
                                 attrib.hasAttribute("maxCode") ? attrib.value("maxCode").toString() : "",
                                 attrib.hasAttribute("divider") ? attrib.value("divider").toDouble() : 1,
                                 showMin, showMax);


    if(attrib.hasAttribute("min")) {
        devLimit->setMinValue(attrib.value("min").toDouble());
    }

    if(attrib.hasAttribute("max")) {
        devLimit->setMaxValue(attrib.value("max").toDouble());
    }

    if(attrib.hasAttribute("upper")) {
        devLimit->setUpperValue(attrib.value("upper").toDouble());
    }

    if(attrib.hasAttribute("bottom")) {
        devLimit->setBottomValue(attrib.value("bottom").toDouble());
    }

    device->limits.append(devLimit);
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

    device->calibrate.append(tmpStruct);

}

void xmlReader::parseButtons() {
    doubleMaskCommand_t tmp;
    QXmlStreamAttributes attrib = xml.attributes();
    QString tmpName = "noname";
    tmp.mask = 0;

//    btn->setStyleSheet("QPushButton {\n	color: #000;\n	border: 1px solid rgb(31,31,31);\n	border-radius: 4px;\n	padding: 3px 20px;\n	background-color: rgb(189, 1, 2);\n}\n\nQPushButton::checked {\n	background-color: rgb(0, 102, 52);\n}");

    if(attrib.hasAttribute("name")) {
        tmpName = attrib.value("name").toString();
//        btn->setText(tmpName);
        if(tmpName == "laser") {
            device->hasLaser = true;
        } else if (tmpName == "tec") {
            device->hasTEC = true;
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

    device->stateButtons.insert(tmpName, tmp);
}

void xmlReader::parseSpecialParam() {
//    while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "specParam")) {
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
        device->specialParameters.append(tmp);

//        xml.readNext();
//    }
}

//QVector<quint16>& xmlReader::getavailableDevices() {
        //return availableDevices;
//}
