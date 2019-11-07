#include "xmlreader.h"
#include "command.h"
#include "signedcommand.h"

xmlReader::xmlReader(QObject *parent) : QObject(parent)
{
    file = nullptr;
}

void xmlReader::setDeviceOptions(device_t &device, quint16 dev_id) {
    this->device = &device;
    device.hasLaser = false;
    device.hasTEC = false;
    device.laserOn = false;
    device.startTECwithLaser = false;
    deviceId = dev_id;
    device.stopCommandDelay = STOP_COMMAND_DELAY_DEFAULT;
    device.minCommDelay = COM_COMMAND_MIN_SEND_DELAY;
    device.maxCommDelay = COM_COMMAND_MAX_SEND_DELAY;
}

void xmlReader::setDeviceList(QList<availableDev_t>& listPtr) {
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
    if(!file->exists()) {
        emit errorHandler("CONFIG: " + file->errorString());
        return false;
    }
    if(!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit errorHandler("CONFIG: " + file->errorString());
        return false;
    }

    return true;
}

void xmlReader::parseCommonConfig(QString fileName, QList<availableDev_t>& deviceList, QList<uint> &baudList) {
    setConfigFile(fileName);
    setDeviceList(deviceList);
    setBaudsList(baudList);
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
                    devStruct.id = static_cast<quint16>(attrib.value("id").toUInt(nullptr, 16));
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
            if(xname == "Config") break;
        }

        if(xml.hasError()) {
            emit errorHandler("Program CONFIG: " + xml.errorString());
            break;
        }
    }

    emit endOfLoadingProgramConfig(!xml.hasError());

    file->close();
    file->deleteLater();
}

void xmlReader::parseDeviceConfig(QString fileName, device_t &devicePtr, quint16 dev_id) {
    setConfigFile(fileName);
    setDeviceOptions(devicePtr, dev_id);
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

            else if (xname == "Content") {
                parseContent();
//                currDevIndex++;
            } else if (xname == "Limits") {
                parseLimits();
            } else if (xname == "CalibrationKoefs") {
                parseCalibration();
            } else if (xname == "Commands") {
                parseCommands();
            } else if (xname == "ParamControls") {
                parseControls();
            } else if (xname == "Leds") {
                parseLeds();
            }  else if (xname == "BinaryOptions") {
                parseBinaryOptions();
            } else if (xname == "Buttons") {
                parseButtons();
            } else {
//                xml.skipCurrentElement();
            }
        } else if(token == QXmlStreamReader::EndElement) {
            if(xml.name() == "Device" && isDeviceFound) break;
            if(xml.name() == "Config") break;
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
        quint16 currID = static_cast<quint16>(attrib.value("id").toUInt(nullptr, 16));
        if(currID != deviceId) return false;
        device->deviceID = currID;
        if (attrib.hasAttribute("name")) device->devName = attrib.value("name").toString();
        if(attrib.hasAttribute("minCommandDelayMs")) {
            device->minCommDelay = attrib.value("minCommandDelayMs").toUInt();
        }

        if(attrib.hasAttribute("maxCommandDelayMs")) {
            device->maxCommDelay = attrib.value("maxCommandDelayMs").toUInt();
        }

        if(attrib.hasAttribute("stopCommandDelayMs")) {
            device->stopCommandDelay = attrib.value("stopCommandDelayMs").toUInt();
        }

        return true;
    }
    return false;
}

void xmlReader::parseContent() {
    while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "Content")) {
        if(xml.name() == "Image") {
            device->image = xml.readElementText();
        } else if(xml.name() == "Description") {
            device->description = xml.readElementText();
        } else if(xml.name() == "Link") {
            device->link = xml.readElementText();
        }
        xml.readNext();
    }
}

void xmlReader::parseCommands() {
    while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "Commands")) {
        xml.readNext();
        if(!(xml.name() == "Command" && xml.tokenType() == QXmlStreamReader::StartElement)) continue;
        QXmlStreamAttributes attrib = xml.attributes();

        QString code = (attrib.hasAttribute("code")) ? attrib.value("code").toString() : "";
        if(code.isEmpty()) continue;

        double divider = (attrib.hasAttribute("divider")) ? attrib.value("divider").toDouble() : 1;

        quint8 interval = (attrib.hasAttribute("interval")) ? attrib.value("interval").toUInt() : 1;
        interval = qBound(MIN_COM_INTERVAL_COUNTER, interval, MAX_COM_INTERVAL_COUNTER);

        bool isSigned = attrib.hasAttribute("isSigned");

        if(isSigned) {
            device->commands.insert(code, new SignedCommand(code, divider, interval));
        } else {
            device->commands.insert(code, new Command(code, divider, interval));
        }

    }
}

void xmlReader::parseControls() {
    while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "ParamControls")) {
        if(xml.name() == "Param" && xml.tokenType() == QXmlStreamReader::StartElement)
            parseParam();
        xml.readNext();
    }
}

void xmlReader::parseLeds() {
    leds_t *led = nullptr;
    while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "Leds")) {
        if(xml.name() == "Led") {
            if(xml.tokenType() == QXmlStreamReader::StartElement) {
                led = new leds_t;
                QXmlStreamAttributes attrib = xml.attributes();
                if(attrib.hasAttribute("label")) {
                    led->label = attrib.value("label").toString();
                }
            } else if (xml.tokenType() == QXmlStreamReader::EndElement) {
                device->leds.append(*led);
                led = nullptr;
            }
        } else if (xml.name() == "LedMask") {
            parseLedMask(led);
        }
        xml.readNext();
    }
}

void xmlReader::parseLedMask(leds_t* ledPtr) {
    if(ledPtr == nullptr) return;

    ledMask_t ledMask;
    ledMask.message.clear();
    ledMask.activeColor = LEDS_DEFAULT_ACTIVE_COLOR;
    ledMask.defaultColor = LEDS_DEFAULT_COLOR;
    QXmlStreamAttributes attrib = xml.attributes();
    if(attrib.hasAttribute("code")) {
        ledMask.command = attrib.value("code").toString();
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
    if(!maskMsg.isEmpty()) ledMask.message = maskMsg;

    ledPtr->masks.append(ledMask);
}

void xmlReader::parseParam() {
    QXmlStreamAttributes attrib = xml.attributes();

    bool isTemperatureFlag = false;
    QString unit = "";
    QString title = "no name";
    QString minCode, maxCode, valueCode, realCode;
    minCode = maxCode = valueCode = realCode = "";

    if(attrib.hasAttribute("isTemperature")) {
        isTemperatureFlag = (attrib.value("isTemperature").toUInt() == 0) ? false : true;
    }

    if(attrib.hasAttribute("unit")) {
        unit = attrib.value("unit").toString();
        unit.replace("(deg)", QString::fromRawData(new QChar('\260'), 1));
    }

    if(attrib.hasAttribute("min")) {
        minCode = attrib.value("min").toString();
    }

    if(attrib.hasAttribute("max")) {
        maxCode = attrib.value("max").toString();
    }

    if(attrib.hasAttribute("value")) {
        valueCode = attrib.value("value").toString();
    }

    if(attrib.hasAttribute("real")) {
        realCode = attrib.value("real").toString();
    }

    title = xml.readElementText();

    double divider = (device->commands.contains(valueCode)) ? device->commands.value(valueCode)->getDivider() : 1;
    double realDivider = (device->commands.contains(realCode)) ? device->commands.value(realCode)->getDivider() : 1;

    ParameterController* parameterController = new ParameterController(title, unit, minCode, maxCode, valueCode, realCode, divider, realDivider, isTemperatureFlag);

    device->paramWidgets.append(parameterController);

}

void xmlReader::parseLimits() {
    while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "Limits")) {
        xml.readNext();
        if(!(xml.name() == "Limit" && xml.tokenType() == QXmlStreamReader::StartElement)) continue;
        //    if(!currDeviceFound) return;
        QXmlStreamAttributes attrib = xml.attributes();
        // If there is no both codes (min and max) then it is a wrong configuration of limit
        if(!(attrib.hasAttribute("minCode") || attrib.hasAttribute("maxCode") || attrib.hasAttribute("min") || attrib.hasAttribute("max"))) return;

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

        double divider = 1;
        QString dividerParam;
        dividerParam.clear();

        if(attrib.hasAttribute("minCode")) {
            dividerParam = attrib.value("minCode").toString();
        } else if (attrib.hasAttribute("maxCode")) {
            dividerParam = attrib.value("maxCode").toString();
        }

        if(!dividerParam.isEmpty()) {
            if(device->commands.contains(dividerParam)) {
                divider = device->commands.value(dividerParam)->getDivider();
            }
        }

        DeviceLimit* devLimit = new DeviceLimit(xml.readElementText(),
                                                attrib.hasAttribute("unit") ? attrib.value("unit").toString() : "",
                                                attrib.hasAttribute("bottomCode") ? attrib.value("bottomCode").toString() : "",
                                                attrib.hasAttribute("upperCode") ? attrib.value("upperCode").toString() : "",
                                                attrib.hasAttribute("minCode") ? attrib.value("minCode").toString() : "",
                                                attrib.hasAttribute("maxCode") ? attrib.value("maxCode").toString() : "",
                                                divider, showMin, showMax);


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
}

void xmlReader::parseCalibration() {
    while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "CalibrationKoefs")) {
        xml.readNext();
        if(!(xml.name() == "Calibrate" && xml.tokenType() == QXmlStreamReader::StartElement)) continue;
        QXmlStreamAttributes attrib = xml.attributes();
        calibration_t calibrate;
        calibrate.code = "";
        calibrate.title = "No name";
        calibrate.min = 9500;
        calibrate.max = 10500;
        calibrate.divider = 100;

        if(attrib.hasAttribute("code")) {
            calibrate.code = attrib.value("code").toString();
            if(device->commands.contains(calibrate.code)) {
                calibrate.divider = device->commands.find(calibrate.code).value()->getDivider();
            }
        }

        if(attrib.hasAttribute("min")) {
            calibrate.min = attrib.value("min").toInt();
        }

        if(attrib.hasAttribute("max")) {
            calibrate.max = attrib.value("max").toInt();
        }

        calibrate.title = xml.readElementText();

        device->calCoefs.append(calibrate);

    }
}

void xmlReader::parseButtons() {
    while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "Buttons")) {
        xml.readNext();
        if(!(xml.name() == "Button" && xml.tokenType() == QXmlStreamReader::StartElement)) continue;
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
}

void xmlReader::parseBinaryOptions() {
    while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "BinaryOptions")) {
        xml.readNext();
        if(!(xml.name() == "CheckBox" && xml.tokenType() == QXmlStreamReader::StartElement)) continue;

        binOption_t binOption;
        QXmlStreamAttributes attrib = xml.attributes();

        if(attrib.hasAttribute("code")) {
            binOption.code = attrib.value("code").toString();
        }
        if(attrib.hasAttribute("offCommand")) {
            binOption.offCommand = attrib.value("offCommand").toString();
        }
        if(attrib.hasAttribute("onCommand")) {
            binOption.onCommand = attrib.value("onCommand").toString();
        }
        if(attrib.hasAttribute("mask")) {
            binOption.mask = attrib.value("mask").toUInt(nullptr, 16);
        } else {
            binOption.mask = 0;
        }


        QCheckBox *binOptionCb = new QCheckBox();
        QString label = xml.readElementText();
        binOptionCb->setText(label);
        binOption.label = label;
        binOptionCb->setStyleSheet("border: none;");
        binOption.checkBox = binOptionCb;
        device->binOptions.append(binOption);

    }
}
