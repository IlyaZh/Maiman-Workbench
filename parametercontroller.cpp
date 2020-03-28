#include "parametercontroller.h"
#include "math.h"

ParameterController::ParameterController(QString title, Command* minComm, Command* maxComm, Command* valueComm, Command* realComm, QWidget *parent) : QWidget(parent)
{
    this->minComm = minComm;
    this->maxComm = maxComm;
    this->realComm = realComm;
    this->valueComm = valueComm;

    prepareBigWidget();
    prepareCompactViewWidget();
    prepareTextWidget();

    // Divider
    if(realComm != nullptr)
        precisionOfRealValue = qRound(log10(realComm->getDivider()));
    if(valueComm != nullptr)
        precisionOfValue = qRound(log10(valueComm->getDivider()));
    validator.setDecimals(precisionOfValue);
    ui_currValueLine->setValidator(&validator);
    ui_currValueCompactLine->setValidator(&validator);
    // end divider

    // title
    this->title = title;
    ui_titleLabel->setText(title);
    ui_titleCompactLabel->setText(title);
    titleLabel->setText(title);
    // end title

    setUnit();

    hideRealValue(realComm == nullptr);

    setEditLineDefaultState();
    QMetaObject::connectSlotsByName(this);

    if(this->minComm != nullptr) {
        connect(this->minComm, &Command::valueChanged, this, &ParameterController::setMin);
    }
    if(this->maxComm != nullptr) {
        connect(this->maxComm, &Command::valueChanged, this, &ParameterController::setMax);
    }
    if(this->valueComm != nullptr) {
        connect(this->valueComm, &Command::valueChanged, this, &ParameterController::setSentValue);
    }
    if(this->realComm != nullptr) {
        connect(this->realComm, &Command::valueChanged, this, &ParameterController::setRealValue);
    }

    setPinState(false);
    isUserEdited = false;
}


ParameterController::~ParameterController() {
    if(compactWidget) delete compactWidget;
    if(bigWidget) delete bigWidget;
    if(textWidget) delete textWidget;
}

bool ParameterController::isOnlyMeasured() {
    return (realComm != nullptr) && (valueComm == nullptr);
}

bool ParameterController::hasRealCommand() {
    return realComm != nullptr;
}

void ParameterController::prepareBigWidget() {
    QFile file(":/forms/parambigwidget.ui");
    file.open(QFile::ReadOnly);
    bigWidget = loader.load(&file, this);
    file.close();

    ui_titleLabel = bigWidget->findChild<QLabel*>("titleLabel");
    ui_minLabel = bigWidget->findChild<QLabel*>("minLabel");
    ui_maxLabel = bigWidget->findChild<QLabel*>("maxLabel");
    ui_setValueLabel = bigWidget->findChild<QLabel*>("setValueLabel");
    ui_typeLabel = bigWidget->findChild<QLabel*>("typeLabel");
    ui_currValueUnitLabel = bigWidget->findChild<QLabel*>("currValueUnitLabel");
    ui_currValueLine = bigWidget->findChild<QLineEdit*>("currValueLine");
    ui_minUnitProgressLabel = bigWidget->findChild<QLabel*>("minUnitProgressLabel");
    ui_maxUnitProgressLabel = bigWidget->findChild<QLabel*>("maxUnitProgressLabel");
    ui_measuredValueBar = bigWidget->findChild<QProgressBar*>("measuredValueBar");
    ui_valueSlider = bigWidget->findChild<QSlider*>("valueSlider");
    ui_minusButton = bigWidget->findChild<QPushButton*>("minusButton");
    ui_plusButton = bigWidget->findChild<QPushButton*>("plusButton");
    ui_pinButton = bigWidget->findChild<QPushButton*>("pinButton");

    QHBoxLayout *layout = new QHBoxLayout();
    ui_sendValueButton = new QPushButton;
    ui_sendValueButton->setText("set");
    ui_sendValueButton->setCheckable(true);
    ui_sendValueButton->setHidden(true);
    ui_sendValueButton->setStyleSheet("QPushButton { \
                                      color: rgb(255, 255, 255); \
                                      background-color: rgb(51, 51, 51); \
                                      border: 1px solid #999999; \
                                      border-radius: 4px; \
                                      padding: 4px; \
                                  }");
    ui_sendValueButton->setFont(APPLICATION_DEFAULT_FONT);
//    ui_sendValueButton->setMaximumSize(QSize(15,15));
    layout->addWidget(ui_sendValueButton, 0, Qt::AlignRight);
    connect(ui_sendValueButton, SIGNAL(clicked(bool)), this, SLOT(sendValueSlot()));
    ui_currValueLine->setLayout(layout);

    bigWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
}

void ParameterController::prepareCompactViewWidget() {
    //    load widget for compact view
        QFile file;
        file.setFileName(":/forms/paramcompactwidget.ui");
        file.open(QFile::ReadOnly);
        compactWidget = loader.load(&file, this);
        file.close();

        ui_currValueCompactLine = compactWidget->findChild<QLineEdit*>("currValueCompactLine");
        ui_minCompactLabel = compactWidget->findChild<QLabel*>("minCompactLabel");
        ui_maxCompactLabel = compactWidget->findChild<QLabel*>("maxCompactLabel");
        ui_realValueUnitCompactLabel = compactWidget->findChild<QLabel*>("realValueUnitCompactLabel");
        ui_titleCompactLabel = compactWidget->findChild<QLabel*>("titleCompactLabel");
        ui_minusCompactButton = compactWidget->findChild<QPushButton*>("minusCompactButton");
        ui_plusCompactButton = compactWidget->findChild<QPushButton*>("plusCompactButton");

        QHBoxLayout *layoutCompact = new QHBoxLayout();
        ui_sendValueCompactButton = new QPushButton();
        ui_sendValueCompactButton->setText("set");
        ui_sendValueCompactButton->setCheckable(true);
    //    ui_sendValueCompactButton->setMaximumSize(QSize(15,15));
        layoutCompact->addWidget(ui_sendValueCompactButton, 0, Qt::AlignRight);
        ui_sendValueCompactButton->setHidden(true);
        ui_sendValueCompactButton->setStyleSheet("QPushButton { \
                                          color: rgb(255, 255, 255); \
                                          background-color: rgb(51, 51, 51); \
                                          border: 1px solid #999999; \
                                          border-radius: 4px; \
                                          padding: 4px; \
                                      }");
        ui_sendValueCompactButton->setFont(APPLICATION_DEFAULT_FONT);
        connect(ui_sendValueCompactButton, SIGNAL(clicked(bool)), this, SLOT(sendValueCompactSlot()));
        ui_currValueCompactLine->setLayout(layoutCompact);

        compactWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    //    end of loading widget for compact view
}

void ParameterController::prepareTextWidget() {
    //    load widget for text view
    QFile file;
    file.setFileName(":/forms/paramtextwidget.ui");
    file.open(QFile::ReadOnly);
    textWidget = loader.load(&file, this);
    file.close();

    titleLabel = textWidget->findChild<QLabel*>("titleLabel");
    valueLabel = textWidget->findChild<QLabel*>("valueLabel");
    unitLabel = textWidget->findChild<QLabel*>("unitLabel");
    //    end of loading widget for text view
}

QWidget* ParameterController::loadWidget() {
    return bigWidget;
}

QWidget* ParameterController::loadCompactWidget() {
    return compactWidget;
}

QWidget* ParameterController::loadTextWidget() {
    return textWidget;
}

void ParameterController::hideRealValue(bool state) {
    hideReal = state;

    if(hideReal) {
        // только установленное значение
        ui_setValueLabel->setStyleSheet("color: rgb(17, 17, 17);");
        ui_realValueUnitCompactLabel->setStyleSheet("color: rgb(17, 17, 17);");

        ui_typeLabel->setText(tr("Set"));
        ui_measuredValueBar->setValue(ui_measuredValueBar->minimum());

    } else {
        // присутствуют оба значения: измеренное и установленное
        ui_setValueLabel->setStyleSheet("color: rgb(180, 180, 180);");
        ui_realValueUnitCompactLabel->setStyleSheet("color: rgb(180, 180, 180);");
        ui_typeLabel->setText(tr("Real"));
        ui_setValueLabel->setText("set=" + wlocale.toString(static_cast<double>(ui_valueSlider->value())/realComm->getDivider(), DOUBLE_FORMAT, precisionOfRealValue) + realComm->getUnit());
        // Устанавлиавем красный цвет прогресс-бара
        ui_measuredValueBar->setStyleSheet(" QProgressBar { \
                                                 border: 1px solid rgb(25,25,25); \
                                                 background: rgb(25,25,25); \
                                                 border-radius: 5px; \
                                                 width: 3px; \
                                             } \
                                             QProgressBar::chunk { \
                                                 background: rgb(254, 26, 6); \
                                             }");
    }
    setUnit();
}

void ParameterController::setUnit() {
//    if(isTemperature()) {
//        unit += settings.getTemperatureSymbol();
//        unitLabel->setText(unit);

//        ui_maxLabel->setText("max=" + wlocale.toString(max, DOUBLE_FORMAT, precisionOfValue) + unit);
//        ui_maxCompactLabel->setText("max=" + wlocale.toString(max, DOUBLE_FORMAT, precisionOfValue) + unit);
//        ui_minLabel->setText("min=" + wlocale.toString(min, DOUBLE_FORMAT, precisionOfValue) + unit);
//        ui_minCompactLabel->setText("min=" + wlocale.toString(min, DOUBLE_FORMAT, precisionOfValue) + unit);

//        ui_setValueLabel->setText("set=" + wlocale.toString(currValue, DOUBLE_FORMAT, precisionOfValue) + unit);
//        ui_realValueUnitCompactLabel->setText("real=" + wlocale.toString(realValue, DOUBLE_FORMAT, precisionOfRealValue) + unit);
//        ui_currValueUnitLabel->setText(wlocale.toString(realValue, DOUBLE_FORMAT, precisionOfRealValue) + unit);
//        ui_maxUnitProgressLabel->setText(wlocale.toString(max, DOUBLE_FORMAT, precisionOfValue) + unit);
//        ui_minUnitProgressLabel->setText(wlocale.toString(min, DOUBLE_FORMAT, precisionOfValue) + unit);
//    } else {
        if(valueComm != nullptr)
            unitLabel->setText(valueComm->getUnit());
        else if(realComm != nullptr)
            unitLabel->setText(realComm->getUnit());
//    }
}

void ParameterController::setMax() {
    if(minComm == nullptr || maxComm == nullptr || valueComm == nullptr) return;

    QString strValue = wlocale.toString(maxComm->getValue(), DOUBLE_FORMAT, precisionOfValue);
    QString strValueUnit = strValue + valueComm->getUnit();

    ui_maxLabel->setText("max=" + strValueUnit);
    ui_maxCompactLabel->setText("max=" + strValueUnit);
    ui_valueSlider->setMaximum(maxComm->getIValue());
    ui_measuredValueBar->setMaximum(maxComm->getIValue());
    ui_maxUnitProgressLabel->setText(strValueUnit);
}

void ParameterController::setMin() {
    if(minComm == nullptr || maxComm == nullptr || valueComm == nullptr) return;

    QString strValue = wlocale.toString(minComm->getValue(), DOUBLE_FORMAT, precisionOfValue);
    QString strValueUnit = strValue + valueComm->getUnit();

    ui_minLabel->setText("min=" + strValueUnit);
    ui_minCompactLabel->setText("min=" + strValueUnit);
    ui_valueSlider->setMinimum(minComm->getIValue());
    ui_measuredValueBar->setMinimum(minComm->getIValue());
    ui_minUnitProgressLabel->setText(strValueUnit);
}

void ParameterController::setPinState(bool val) {
    pinState = val;
}

bool ParameterController::getPinState() {
    return pinState;
}

bool ParameterController::getEnableState() {
    return ui_currValueLine->isEnabled();
}

void ParameterController::setRealValue() {
    if(realComm == nullptr) return;

    QString strValue = wlocale.toString(realComm->getValue(), DOUBLE_FORMAT, precisionOfRealValue);
    QString strValueUnit = strValue + realComm->getUnit();

    ui_measuredValueBar->setValue(static_cast<int>(realComm->getIValue()));
    ui_measuredValueBar->setStyleSheet(" QProgressBar { \
                                     border: 1px solid rgb(25,25,25); \
                                     background: rgb(25,25,25); \
                                     border-radius: 5px; \
                                     width: 3px; \
                                 } \
                                 QProgressBar::chunk { \
                                     background: rgb(254, 26, 6); \
                                 }");
    if (!isOnlyMeasured()) {
        ui_currValueUnitLabel->setText(strValueUnit);
        ui_realValueUnitCompactLabel->setText("real=" + strValueUnit);
    }
    valueLabel->setText(strValue);
    setUnit();
}

void ParameterController::setSentValue() {
    if(!isUserEdited) {
        ui_valueSlider->setValue(static_cast<int>(valueComm->getIValue()));
    }
}

// slots

// Try to change this slot for sliderMoved() or sliderReleased()
void ParameterController::on_valueSlider_valueChanged(int value) {
    if(valueComm == nullptr) return;
    double currValue = static_cast<double>(value/valueComm->getDivider());
    ui_sendValueButton->hide();
    ui_sendValueCompactButton->hide();

    QString strValue = wlocale.toString(currValue, DOUBLE_FORMAT, precisionOfValue);
    QString strValueUnit = strValue + valueComm->getUnit();

    if(!ui_currValueLine->hasFocus())
        ui_currValueLine->setText(strValue);
    if(!ui_currValueCompactLine->hasFocus())
        ui_currValueCompactLine->setText(strValue);

    if(hideReal)
        ui_currValueUnitLabel->setText(strValueUnit);
    else
        ui_setValueLabel->setText("set=" + strValueUnit);

    if(valueLabel && unitLabel) {
        valueLabel->setText(strValue);
        if(valueComm->isTemperature())
            unitLabel->setText(valueComm->getUnit());
    }
    setUnit();
}

void ParameterController::on_valueSlider_sliderPressed() {
    isUserEdited = true;
    /*
     * Делаем такой флаг, чтобы избежать отправки огромного количества комманд
     * изменения состояния параметра, до тех пор, пока пользователь не отпустит
     * ползунок
     */
}

void ParameterController::on_valueSlider_sliderMoved(int value) {
    if(valueComm == nullptr) return;
    double currValue = static_cast<double>(value/valueComm->getDivider());
    ui_sendValueButton->hide();
    ui_sendValueCompactButton->hide();

    QString strValue = wlocale.toString(currValue, DOUBLE_FORMAT, precisionOfValue);
    QString strValueUnit = strValue + valueComm->getUnit();

    if(!ui_currValueLine->hasFocus())
        ui_currValueLine->setText(strValue);
    if(!ui_currValueCompactLine->hasFocus())
        ui_currValueCompactLine->setText(strValue);

    if(hideReal)
        ui_currValueUnitLabel->setText(strValueUnit);
    else
        ui_setValueLabel->setText("set=" + strValueUnit);

    setUnit();
}

void ParameterController::on_valueSlider_sliderReleased() {
    if(valueComm == nullptr) return;
    if(ui_currValueLine->hasFocus()) ui_currValueLine->clearFocus();
    preparedCommand = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(valueComm->getCode()).arg(ui_valueSlider->value(), 4, 16, QChar('0'));
    emit changeValue(preparedCommand);
    isUserEdited = false;
}

void ParameterController::on_minusButton_released() {
    if(valueComm == nullptr) return;
    if(ui_currValueLine->hasFocus()) ui_currValueLine->clearFocus();
    quint16 value = ui_valueSlider->value();
    if(value > ui_valueSlider->minimum())
        value--;
    ui_valueSlider->setValue(value);
    preparedCommand = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(valueComm->getCode()).arg(value, 4, 16, QChar('0'));
    emit changeValue(preparedCommand);
    ui_sendValueButton->hide();
//    ui_sendValueCompactButton->hide();
}

void ParameterController::on_plusButton_released() {
    if(valueComm == nullptr) return;
    if(ui_currValueLine->hasFocus()) ui_currValueLine->clearFocus();
    quint16 value = ui_valueSlider->value();
    if(value < ui_valueSlider->maximum())
        value++;
    ui_valueSlider->setValue(value);
    preparedCommand = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(valueComm->getCode()).arg(value, 4, 16, QChar('0'));
    emit changeValue(preparedCommand);
    ui_sendValueButton->hide();
//    ui_sendValueCompactButton->hide();
}

void ParameterController::on_pinButton_released() {
    setPinState(!pinState);
}

void ParameterController::on_currValueLine_returnPressed() {
    if(valueComm == nullptr || maxComm == nullptr || minComm == nullptr) return;
    double currValue = wlocale.toDouble(ui_currValueLine->text());
    double min = minComm->getValue();
    double max = maxComm->getValue();

    if(currValue > max) {
        currValue = max;
        setEditLineRedColor();
        QTimer::singleShot(CURR_VALUE_BG_ERROR_TIMEOUT, this, SLOT(setEditLineDefaultState()));
    } else if (currValue < min) {
        currValue = min;
        setEditLineRedColor();
        QTimer::singleShot(CURR_VALUE_BG_ERROR_TIMEOUT, this, SLOT(setEditLineDefaultState()));
    }

    if(ui_currValueLine->hasFocus()) ui_currValueLine->clearFocus();
    quint16 iCurrValue = qRound(currValue*valueComm->getDivider());
    ui_valueSlider->setValue(iCurrValue);
    ui_sendValueButton->hide();
    preparedCommand = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(valueComm->getCode()).arg(iCurrValue, 4, 16, QChar('0'));
    emit changeValue(preparedCommand);
}

void ParameterController::on_currValueLine_textEdited() {
    ui_sendValueButton->setChecked(false);
    ui_sendValueButton->show();
}

void ParameterController::sendValueSlot() {
    if(valueComm == nullptr || maxComm == nullptr || minComm == nullptr) return;
    if(ui_sendValueButton->isHidden()) return;

    double currValue = wlocale.toDouble(ui_currValueLine->text());
    double min = minComm->getValue();
    double max = maxComm->getValue();


    if(currValue > max) {
        currValue = max;
        setEditLineRedColor();
        QTimer::singleShot(CURR_VALUE_BG_ERROR_TIMEOUT, this, SLOT(setEditLineDefaultState()));
    } else if (currValue < min) {
        currValue = min;
        setEditLineRedColor();
        QTimer::singleShot(CURR_VALUE_BG_ERROR_TIMEOUT, this, SLOT(setEditLineDefaultState()));
    }
    quint16 iCurrValue = qRound(currValue*valueComm->getDivider());

    if(ui_currValueLine->hasFocus()) ui_currValueLine->clearFocus();
    ui_valueSlider->setValue(iCurrValue);
    ui_sendValueButton->hide();
    preparedCommand = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(valueComm->getCode()).arg(iCurrValue, 4, 16, QChar('0'));
    emit changeValue(preparedCommand);

}

/*
 * COMPACT MODE SLOTS AND SIGNLAS
 */

void ParameterController::on_currValueCompactLine_returnPressed() {
    if(valueComm == nullptr || maxComm == nullptr || minComm == nullptr) return;
    if(ui_currValueCompactLine->hasFocus()) ui_currValueCompactLine->clearFocus();

    double currValue = wlocale.toDouble(ui_currValueCompactLine->text());
    double min = minComm->getValue();
    double max = maxComm->getValue();

    if(currValue > max) {
        currValue = max;
        setEditLineRedColor();
        QTimer::singleShot(CURR_VALUE_BG_ERROR_TIMEOUT, this, SLOT(setEditLineDefaultState()));
    } else if (currValue < min) {
        currValue = min;
        setEditLineRedColor();
        QTimer::singleShot(CURR_VALUE_BG_ERROR_TIMEOUT, this, SLOT(setEditLineDefaultState()));
    }
    quint16 value = static_cast<quint16>(qRound(currValue*valueComm->getDivider()));
    ui_valueSlider->setValue(value);
    ui_sendValueCompactButton->hide();
    preparedCommand = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(valueComm->getCode()).arg(value, 4, 16, QChar('0'));
    emit changeValue(preparedCommand);
}

void ParameterController::on_currValueCompactLine_textEdited() {
    ui_sendValueCompactButton->setChecked(false);
    ui_sendValueCompactButton->show();
}

void ParameterController::on_minusCompactButton_released() {
    if(minComm == nullptr || valueComm == nullptr) return;

    if(ui_currValueCompactLine->hasFocus()) ui_currValueCompactLine->clearFocus();

    double currValue = wlocale.toDouble(ui_currValueCompactLine->text());
    double min = minComm->getValue();

    if(currValue > min)
        ui_valueSlider->setValue(ui_valueSlider->value()-1);
    preparedCommand = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(valueComm->getCode()).arg(qRound(currValue*valueComm->getDivider()), 4, 16, QChar('0'));
    emit changeValue(preparedCommand);
    ui_sendValueCompactButton->hide();
}

void ParameterController::on_plusCompactButton_released() {
    if(maxComm == nullptr || valueComm == nullptr) return;
    if(ui_currValueCompactLine->hasFocus()) ui_currValueCompactLine->clearFocus();

    double currValue = wlocale.toDouble(ui_currValueCompactLine->text());
    double max = maxComm->getValue();

    if(currValue < max)
        ui_valueSlider->setValue(ui_valueSlider->value()+1);
    preparedCommand = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(valueComm->getCode()).arg(qRound(currValue*valueComm->getDivider()), 4, 16, QChar('0'));
    emit changeValue(preparedCommand);
    ui_sendValueCompactButton->hide();
}

void ParameterController::sendValueCompactSlot() {
    if(minComm == nullptr || maxComm == nullptr || valueComm == nullptr) return;
    if(ui_sendValueCompactButton->isHidden()) return;
    if(ui_currValueCompactLine->hasFocus()) ui_currValueCompactLine->clearFocus();

    double currValue = wlocale.toDouble(ui_currValueCompactLine->text());
    double min = minComm->getValue();
    double max = maxComm->getValue();

    if(currValue > max) {
        currValue = max;
        setEditLineRedColor();
        QTimer::singleShot(CURR_VALUE_BG_ERROR_TIMEOUT, this, SLOT(setEditLineDefaultState()));
    } else if (currValue < min) {
        currValue = min;
        setEditLineRedColor();
        QTimer::singleShot(CURR_VALUE_BG_ERROR_TIMEOUT, this, SLOT(setEditLineDefaultState()));
    }

    ui_valueSlider->setValue(static_cast<quint16> (qRound(currValue*valueComm->getDivider())));
    ui_sendValueCompactButton->hide();
    preparedCommand = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(valueComm->getCode()).arg(ui_valueSlider->value(), 4, 16, QChar('0'));
    emit changeValue(preparedCommand);
}

void ParameterController::setEditLineRedColor() {
        ui_currValueLine->setStyleSheet("QLineEdit {\n	color: rgb(255, 255, 255);\n	background: rgb(230, 0, 0);\n	border-radius: 5px;\n	padding: 5px 0;\n}\n\nQLineEdit::disabled {\n	background: rgb(180, 180, 180);\n	color: rgb(76, 93, 100);\n}");
        ui_currValueCompactLine->setStyleSheet("QLineEdit {\n	color: rgb(255, 255, 255);\n	background: rgb(230, 0, 0);\n	border-radius: 5px;\n	padding: 5px 0;\n}\n\nQLineEdit::disabled {\n	background: rgb(180, 180, 180);\n	color: rgb(76, 93, 100);\n}");
}
void ParameterController::setEditLineDefaultState() {
    if(valueComm == nullptr) return;

    double currValue = valueComm->getValue();
    QString strValue = wlocale.toString(currValue, DOUBLE_FORMAT, precisionOfValue);

    ui_currValueLine->setStyleSheet("QLineEdit {\n	color: rgb(16, 33, 40);\n	background: rgb(230, 230, 230);\n	border-radius: 5px;\n	padding: 5px 0;\n}\n\nQLineEdit::disabled {\n	background: rgb(180, 180, 180);\n	color: rgb(76, 93, 100);\n}");
    ui_currValueCompactLine->setStyleSheet("QLineEdit {\n	color: rgb(16, 33, 40);\n	background: rgb(230, 230, 230);\n	border-radius: 5px;\n	padding: 5px 0;\n}\n\nQLineEdit::disabled {\n	background: rgb(180, 180, 180);\n	color: rgb(76, 93, 100);\n}");
//    return;
    ui_currValueCompactLine->setText(strValue);
    ui_currValueLine->setText(strValue);
    ui_valueSlider->setValue(qRound(currValue*valueComm->getDivider()));
}

/*
 * END OF COMPACT MODE SLOTS AND SIGNLAS
 */

/*void ParameterController::temperatureIsChanged(QString str) {
    if(str == "F" && isCelsius) {
        // Меняем цельсии на фаренгейты
        isCelsius = false;
//        setMin(convertCelToFar(min));
//        setMax(convertCelToFar(max));
//        setRealValue(convertCelToFar(realValue));
//        setSentValue(convertCelToFar(currValue));
        setUnit(QString::fromRawData(new QChar('\260'), 1));// + str);
    } else if(str == "C" && !isCelsius) {
        // Меняем фаренгейты на цельсии
        isCelsius = true;
//        setMin(convertFarToCel(min));
//        setMax(convertFarToCel(max));
//        setRealValue(convertFarToCel(realValue));
//        setSentValue(convertCelToFar(currValue));
        setUnit(QString::fromRawData(new QChar('\260'), 1));// + str);
    }

}*/

void ParameterController::setEnableState(bool state) {
    ui_valueSlider->setEnabled(state);
    ui_minusButton->setEnabled(state);
    ui_plusButton->setEnabled(state);
    ui_currValueLine->setEnabled(state);
    ui_minusCompactButton->setEnabled(state);
    ui_plusCompactButton->setEnabled(state);
    ui_currValueCompactLine->setEnabled(state);
}

//QString ParameterController::getMinComm() {
//    return minComm;
//}

//QString ParameterController::getMaxComm() {
//    return maxComm;
//}

//QString ParameterController::getRealComm() {
//    return realComm;
//}

//QString ParameterController::getValueComm() {
//    return valueComm;
//}

//bool ParameterController::isTemperature() {
//    return isTemperatureFlag;
//}
