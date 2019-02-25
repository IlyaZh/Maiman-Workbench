#include "parameterform.h"

//ParameterForm::ParameterForm(const paramControls_t &inParam, QWidget *parent) :
ParameterForm::ParameterForm(QWidget *parent) : QWidget(parent)
{
    QUiLoader loader;
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
    ui_readedValueBar = bigWidget->findChild<QProgressBar*>("readedValueBar");
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

//    load widget for compact view
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
//    end of loading widget for compact view

//    load widget for text view
    file.setFileName(":/forms/paramtextwidget.ui");
    file.open(QFile::ReadOnly);
    textWidget = loader.load(&file, this);
    file.close();

    titleLabel = textWidget->findChild<QLabel*>("titleLabel");
    valueLabel = textWidget->findChild<QLabel*>("valueLabel");
    unitLabel = textWidget->findChild<QLabel*>("unitLabel");
//    end of loading widget for text view


    QMetaObject::connectSlotsByName(this);

    if(settings.getTemperatureSymbol() == "C")
        isCelsius = true;
    else if (settings.getTemperatureSymbol() == "F")
        isCelsius = false;

    setPinState(false);
    isUserEdited = true;
    isTemperatureFlag = false;

    bigWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    compactWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    setEditLineDefaultState();
}

ParameterForm::~ParameterForm() {
    if(compactWidget) delete compactWidget;
    if(bigWidget) delete bigWidget;
    if(textWidget) delete textWidget;
}

QWidget* ParameterForm::loadWidget() {
    return bigWidget;
}

QWidget* ParameterForm::loadCompactWidget() {
    return compactWidget;
}

QWidget* ParameterForm::loadTextWidget() {
    return textWidget;
}

//void ParameterForm::updateWidget() {}

//void ParameterForm::deleteWidget() {
//    if(compactWidget != 0) compactWidget->deleteLater();
//    if(bigWidget != 0) bigWidget->deleteLater();
//}

void ParameterForm::setDivider(double val) {
    divider = val;
    QDoubleValidator *validator = new QDoubleValidator();
    validator->setDecimals(qRound(log10(divider)));
    ui_currValueLine->setValidator(validator);
    ui_currValueCompactLine->setValidator(validator);
}

void ParameterForm::setIsTemperatureFlag(bool state) {
    isTemperatureFlag = state;
}

void ParameterForm::hideRealValue(bool state) {
    hideReal = state;
    if(hideReal) {
        // только установленное значение
//        ui_setValueLabel->hide();
        ui_setValueLabel->setStyleSheet("color: rgb(17, 17, 17);");
        ui_realValueUnitCompactLabel->setStyleSheet("color: rgb(17, 17, 17);");

//        ui_currValueLine->setEnabled(false);
        ui_typeLabel->setText(tr("Set"));
        ui_readedValueBar->setValue(ui_readedValueBar->minimum());
//        ui_realValueUnitCompactLabel->hide();
    } else {
        // присутствуют оба значения: измеренное и установленное
//        ui_setValueLabel->show();
//        ui_realValueUnitCompactLabel->show();
        ui_setValueLabel->setStyleSheet("color: rgb(180, 180, 180);");
        ui_realValueUnitCompactLabel->setStyleSheet("color: rgb(180, 180, 180);");
//        ui_currValueLine->setEnabled(true);
        ui_typeLabel->setText(tr("Real"));
        ui_setValueLabel->setText("set=" + wlocale.toString(ui_valueSlider->value()/divider, DOUBLE_FORMAT, log10(divider)) + unit);
        // Устанавлиавем красный цвет прогресс-бара
        ui_readedValueBar->setStyleSheet(" QProgressBar { \
                                                 border: 1px solid rgb(25,25,25); \
                                                 background: rgb(25,25,25); \
                                                 border-radius: 5px; \
                                                 width: 3px; \
                                             } \
                                             QProgressBar::chunk { \
                                                 background: rgb(254, 26, 6); \
                                             }");
    }

//    QGroupBox *gb = bigWidget->findChild<QGroupBox*>("groupBox");
//    if(gb) gb->adjustSize();
//    QWidget *w =  bigWidget->findChild<QWidget*>("parameterForm");
//    if(w) w->adjustSize();
//    gb = compactWidget->findChild<QGroupBox*>("groupBox");
//    if(gb) gb->adjustSize();
//    w =  compactWidget->findChild<QWidget*>("parameterForm");
//    if(w) w->adjustSize();
//    bigWidget->adjustSize();
//    compactWidget->adjustSize();

}

void ParameterForm::setTitle(QString str) {
    title = str;
    ui_titleLabel->setText(str);
    ui_titleCompactLabel->setText(str);
    this->setObjectName(str);
    titleLabel->setText(str);
}

void ParameterForm::setUnit(QString str) {
    unit = str;
    if(isTemperature()) {
        unit += settings.getTemperatureSymbol();
        unitLabel->setText(unit);

        ui_maxLabel->setText("max=" + wlocale.toString(max, DOUBLE_FORMAT, log10(divider)) + unit);
        ui_maxCompactLabel->setText("max=" + wlocale.toString(max, DOUBLE_FORMAT, log10(divider)) + unit);
        ui_minLabel->setText("min=" + wlocale.toString(min, DOUBLE_FORMAT, log10(divider)) + unit);
        ui_minCompactLabel->setText("min=" + wlocale.toString(min, DOUBLE_FORMAT, log10(divider)) + unit);

        ui_setValueLabel->setText("set=" + wlocale.toString(currValue, DOUBLE_FORMAT, log10(divider)) + unit);
        ui_realValueUnitCompactLabel->setText("real=" + wlocale.toString(realValue, DOUBLE_FORMAT, log10(divider)) + unit);
        ui_currValueUnitLabel->setText(wlocale.toString(realValue, DOUBLE_FORMAT, log10(divider)) + unit);
        ui_maxUnitProgressLabel->setText(wlocale.toString(max, DOUBLE_FORMAT, log10(divider)) + unit);
        ui_minUnitProgressLabel->setText(wlocale.toString(min, DOUBLE_FORMAT, log10(divider)) + unit);
    } else {
        unitLabel->setText(unit);
    }
}

void ParameterForm::setMax(double val) {
    max = val;
    isUserEdited = false;

    ui_maxLabel->setText("max=" + wlocale.toString(max, DOUBLE_FORMAT, log10(divider)) + unit);
    ui_maxCompactLabel->setText("max=" + wlocale.toString(max, DOUBLE_FORMAT, log10(divider)) + unit);
    ui_valueSlider->setMaximum(qRound(max*divider));
    ui_readedValueBar->setMaximum(qRound(max*divider));
    ui_maxUnitProgressLabel->setText(wlocale.toString(max, DOUBLE_FORMAT, log10(divider)) + unit);

//    QDoubleValidator *validator = new QDoubleValidator(min, max, DEFAULT_DIGITS_AFTER_POINT);
//    validator->setNotation(QDoubleValidator::StandardNotation);

    if(currValue > max) {
        currValue = max;
        ui_valueSlider->setValue(qRound(currValue*divider));
    }
//    ui_currValueLine->setValidator(validator);
//    ui_currValueCompactLine->setValidator(validator);
}

void ParameterForm::setMin(double val) {
     min = val;
    isUserEdited = false;

    ui_minLabel->setText("min=" + wlocale.toString(min, DOUBLE_FORMAT, log10(divider)) + unit);
    ui_minCompactLabel->setText("min=" + wlocale.toString(min, DOUBLE_FORMAT, log10(divider)) + unit);
    ui_valueSlider->setMinimum(qRound(min*divider));
    ui_readedValueBar->setMinimum(qRound(min*divider));
    ui_minUnitProgressLabel->setText(wlocale.toString(min, DOUBLE_FORMAT, log10(divider)) + unit);

//    QDoubleValidator *validator = new QDoubleValidator(min, max, DEFAULT_DIGITS_AFTER_POINT);
//    validator->setNotation(QDoubleValidator::StandardNotation);

    if(currValue < min) {
        currValue = min;
        ui_valueSlider->setValue(qRound(currValue*divider));
    }
//    ui_currValueLine->setValidator(validator);
//    ui_currValueCompactLine->setValidator(validator);
}

void ParameterForm::setPinState(bool val) {
    pinState = val;
}

bool ParameterForm::getPinState() {
    return pinState;
}

bool ParameterForm::getEnableState() {
    return ui_currValueLine->isEnabled();
}

void ParameterForm::setType(QString str) {
    type = str;
}

void ParameterForm::setRealValue(double value) {
    realValue = value;
    ui_readedValueBar->setValue(qRound(realValue*divider));
    ui_readedValueBar->setStyleSheet(" QProgressBar { \
                                     border: 1px solid rgb(25,25,25); \
                                     background: rgb(25,25,25); \
                                     border-radius: 5px; \
                                     width: 3px; \
                                 } \
                                 QProgressBar::chunk { \
                                     background: rgb(254, 26, 6); \
                                 }");
//    if (isTemperature()) {
        ui_currValueUnitLabel->setText(wlocale.toString(realValue, DOUBLE_FORMAT, log10(divider)) + unit);
        ui_realValueUnitCompactLabel->setText("real=" + wlocale.toString(realValue, DOUBLE_FORMAT, log10(divider)) + unit);
//    }
    valueLabel->setText(wlocale.toString(realValue, DOUBLE_FORMAT, log10(divider)));

}

void ParameterForm::setSentValue(double value) {
    if(!isUserEdited)
        ui_valueSlider->setValue((int) qRound(value*divider));
}

//void ParameterForm::setValueComm(QString str) {
//    valueComm = str;
//}

// slots

// Try to change this slot for sliderMoved() or sliderReleased()
void ParameterForm::on_valueSlider_valueChanged(int value) {
    currValue = (double)value/divider;
    ui_sendValueButton->hide();
    ui_sendValueCompactButton->hide();

    if(!ui_currValueLine->hasFocus())
        ui_currValueLine->setText(wlocale.toString(currValue, DOUBLE_FORMAT, log10(divider)));
    if(!ui_currValueCompactLine->hasFocus())
        ui_currValueCompactLine->setText(wlocale.toString(currValue, DOUBLE_FORMAT, log10(divider)));

    if(hideReal)
        ui_currValueUnitLabel->setText(wlocale.toString(currValue, DOUBLE_FORMAT, log10(divider)) + unit);
    else
        ui_setValueLabel->setText("set=" + wlocale.toString(currValue, DOUBLE_FORMAT, log10(divider)) + unit);

    if(valueLabel && unitLabel) {
        valueLabel->setText(wlocale.toString(realValue, DOUBLE_FORMAT, log10(divider)));
        if(isTemperature())
            unitLabel->setText(unit + settings.getTemperatureSymbol());
    }
}

void ParameterForm::on_valueSlider_sliderPressed() {
    isUserEdited = true;
    /*
     * Делаем такой флаг, чтобы избежать отправки огромного количества комманд
     * изменения состояния параметра, до тех пор, пока пользователь не отпустит
     * ползунок
     */
}

void ParameterForm::on_valueSlider_sliderMoved(int value) {
    currValue = (double) value/divider;
    ui_sendValueButton->hide();
    ui_sendValueCompactButton->hide();

    if(!ui_currValueLine->hasFocus())
        ui_currValueLine->setText(wlocale.toString(currValue, DOUBLE_FORMAT, log10(divider)));
    if(!ui_currValueCompactLine->hasFocus())
        ui_currValueCompactLine->setText(wlocale.toString(currValue, DOUBLE_FORMAT, log10(divider)));

    if(hideReal)
        ui_currValueUnitLabel->setText(wlocale.toString(currValue, DOUBLE_FORMAT, log10(divider)) + unit);
    else
        ui_setValueLabel->setText("set=" + wlocale.toString(currValue, DOUBLE_FORMAT, log10(divider)) + unit);
}

void ParameterForm::on_valueSlider_sliderReleased() {
    if(ui_currValueLine->hasFocus()) ui_currValueLine->clearFocus();
    preparedCommand = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(valueComm).arg(ui_valueSlider->value(), 4, 16, QChar('0'));
    emit changeValue(preparedCommand);
    isUserEdited = false;
}

void ParameterForm::on_minusButton_released() {
    if(ui_currValueLine->hasFocus()) ui_currValueLine->clearFocus();
    quint16 tmp = ui_valueSlider->value();
    if(tmp > ui_valueSlider->minimum())
        ui_valueSlider->setValue(ui_valueSlider->value()-1);
    preparedCommand = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(valueComm).arg(ui_valueSlider->value(), 4, 16, QChar('0'));
    emit changeValue(preparedCommand);
    ui_sendValueButton->hide();
//    ui_sendValueCompactButton->hide();
}

void ParameterForm::on_plusButton_released() {
    if(ui_currValueLine->hasFocus()) ui_currValueLine->clearFocus();
    quint16 tmp = ui_valueSlider->value();
    if(tmp < ui_valueSlider->maximum())
        ui_valueSlider->setValue(ui_valueSlider->value()+1);
    preparedCommand = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(valueComm).arg(ui_valueSlider->value(), 4, 16, QChar('0'));
    emit changeValue(preparedCommand);
    ui_sendValueButton->hide();
//    ui_sendValueCompactButton->hide();
}

void ParameterForm::on_pinButton_released() {
    setPinState(!pinState);
}

void ParameterForm::on_currValueLine_returnPressed() {
    currValue = wlocale.toDouble(ui_currValueLine->text());

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

    ui_valueSlider->setValue((quint16) qRound(currValue*divider));
    ui_sendValueButton->hide();
    preparedCommand = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(valueComm).arg(ui_valueSlider->value(), 4, 16, QChar('0'));
    emit changeValue(preparedCommand);
}

void ParameterForm::on_currValueLine_textEdited() {
    ui_sendValueButton->setChecked(false);
    ui_sendValueButton->show();
}

void ParameterForm::sendValueSlot() {
    if(ui_sendValueButton->isHidden()) return;
    currValue = wlocale.toDouble(ui_currValueLine->text());
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
    ui_valueSlider->setValue((quint16) qRound(currValue*divider));
    ui_sendValueButton->hide();
    preparedCommand = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(valueComm).arg(qRound(currValue*divider), 4, 16, QChar('0'));
    emit changeValue(preparedCommand);

}

/*
 * COMPACT MODE SLOTS AND SIGNLAS
 */

void ParameterForm::on_currValueCompactLine_returnPressed() {
    if(ui_currValueCompactLine->hasFocus()) ui_currValueCompactLine->clearFocus();
    currValue = wlocale.toDouble(ui_currValueCompactLine->text());
    if(currValue > max) {
        currValue = max;
        setEditLineRedColor();
        QTimer::singleShot(CURR_VALUE_BG_ERROR_TIMEOUT, this, SLOT(setEditLineDefaultState()));
    } else if (currValue < min) {
        currValue = min;
        setEditLineRedColor();
        QTimer::singleShot(CURR_VALUE_BG_ERROR_TIMEOUT, this, SLOT(setEditLineDefaultState()));
    }
    quint16 value = qRound(currValue*divider);
    ui_valueSlider->setValue(value);
    ui_sendValueCompactButton->hide();
    preparedCommand = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(valueComm).arg(value, 4, 16, QChar('0'));
    emit changeValue(preparedCommand);
}

void ParameterForm::on_currValueCompactLine_textEdited() {
    ui_sendValueCompactButton->setChecked(false);
    ui_sendValueCompactButton->show();
}

void ParameterForm::on_minusCompactButton_released() {
    if(ui_currValueCompactLine->hasFocus()) ui_currValueCompactLine->clearFocus();
    currValue = wlocale.toDouble(ui_currValueCompactLine->text());
    if(currValue > min)
        ui_valueSlider->setValue(ui_valueSlider->value()-1);
    preparedCommand = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(valueComm).arg(qRound(currValue*divider), 4, 16, QChar('0'));
    emit changeValue(preparedCommand);
    ui_sendValueCompactButton->hide();
}

void ParameterForm::on_plusCompactButton_released() {
    if(ui_currValueCompactLine->hasFocus()) ui_currValueCompactLine->clearFocus();
    currValue = wlocale.toDouble(ui_currValueCompactLine->text());
    if(currValue < max)
        ui_valueSlider->setValue(ui_valueSlider->value()+1);
    preparedCommand = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(valueComm).arg(qRound(currValue*divider), 4, 16, QChar('0'));
    emit changeValue(preparedCommand);
    ui_sendValueCompactButton->hide();
}

void ParameterForm::sendValueCompactSlot() {
    if(ui_sendValueCompactButton->isHidden()) return;
    if(ui_currValueCompactLine->hasFocus()) ui_currValueCompactLine->clearFocus();
    currValue = wlocale.toDouble(ui_currValueCompactLine->text());
    if(currValue > max) {
        currValue = max;
        setEditLineRedColor();
        QTimer::singleShot(CURR_VALUE_BG_ERROR_TIMEOUT, this, SLOT(setEditLineDefaultState()));
    } else if (currValue < min) {
        currValue = min;
        setEditLineRedColor();
        QTimer::singleShot(CURR_VALUE_BG_ERROR_TIMEOUT, this, SLOT(setEditLineDefaultState()));
    }

    ui_valueSlider->setValue((quint16) qRound(currValue*divider));
    ui_sendValueCompactButton->hide();
    preparedCommand = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(valueComm).arg(ui_valueSlider->value(), 4, 16, QChar('0'));
    emit changeValue(preparedCommand);
}

void ParameterForm::setEditLineRedColor() {
        ui_currValueLine->setStyleSheet("QLineEdit {\n	color: rgb(255, 255, 255);\n	background: rgb(230, 0, 0);\n	border-radius: 5px;\n	padding: 5px 0;\n}\n\nQLineEdit::disabled {\n	background: rgb(180, 180, 180);\n	color: rgb(76, 93, 100);\n}");
        ui_currValueCompactLine->setStyleSheet("QLineEdit {\n	color: rgb(255, 255, 255);\n	background: rgb(230, 0, 0);\n	border-radius: 5px;\n	padding: 5px 0;\n}\n\nQLineEdit::disabled {\n	background: rgb(180, 180, 180);\n	color: rgb(76, 93, 100);\n}");
}
void ParameterForm::setEditLineDefaultState() {
        ui_currValueLine->setStyleSheet("QLineEdit {\n	color: rgb(16, 33, 40);\n	background: rgb(230, 230, 230);\n	border-radius: 5px;\n	padding: 5px 0;\n}\n\nQLineEdit::disabled {\n	background: rgb(180, 180, 180);\n	color: rgb(76, 93, 100);\n}");
        ui_currValueCompactLine->setStyleSheet("QLineEdit {\n	color: rgb(16, 33, 40);\n	background: rgb(230, 230, 230);\n	border-radius: 5px;\n	padding: 5px 0;\n}\n\nQLineEdit::disabled {\n	background: rgb(180, 180, 180);\n	color: rgb(76, 93, 100);\n}");
        ui_currValueCompactLine->setText(wlocale.toString(currValue, DOUBLE_FORMAT, log10(divider)));
        ui_currValueLine->setText(wlocale.toString(currValue, DOUBLE_FORMAT, log10(divider)));
        ui_valueSlider->setValue(qRound(currValue*divider));
}

/*
 * END OF COMPACT MODE SLOTS AND SIGNLAS
 */

void ParameterForm::temperatureIsChanged(QString str) {
    if(str == "F" && isCelsius) {
        // Меняем цельсии на фаренгейты
        isCelsius = false;
        setMin(convertCelToFar(min));
        setMax(convertCelToFar(max));
        setRealValue(convertCelToFar(realValue));
        setSentValue(convertCelToFar(currValue));
        setUnit(QString::fromRawData(new QChar('\260'), 1));// + str);
    } else if(str == "C" && !isCelsius) {
        // Меняем фаренгейты на цельсии
        isCelsius = true;
        setMin(convertFarToCel(min));
        setMax(convertFarToCel(max));
        setRealValue(convertFarToCel(realValue));
        setSentValue(convertCelToFar(currValue));
        setUnit(QString::fromRawData(new QChar('\260'), 1));// + str);
    }

}

void ParameterForm::setEnableState(bool state) {
    ui_valueSlider->setEnabled(state);
    ui_minusButton->setEnabled(state);
    ui_plusButton->setEnabled(state);
    ui_currValueLine->setEnabled(state);
    ui_minusCompactButton->setEnabled(state);
    ui_plusCompactButton->setEnabled(state);
    ui_currValueCompactLine->setEnabled(state);
}


/*QLabel* ParameterForm::getTitleLabel() {
//    if (titleLabel) delete titleLabel;
    if(titleLabel == 0) titleLabel = new QLabel();
    titleLabel->setText(ui_titleLabel->text());
    return titleLabel;
}

QLabel* ParameterForm::getValueLabel() {
//    if (valueLabel) delete valueLabel;
    if(valueLabel == 0) valueLabel = new QLabel();
    valueLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    valueLabel->setText(wlocale.toString(realValue, DOUBLE_FORMAT, log10(divider)));
    return valueLabel;
}

QLabel* ParameterForm::getUnitLabel() {
//    if (unitLabel) delete unitLabel;
    if(unitLabel == 0) unitLabel = new QLabel();
    unitLabel->setText(unit);
    unitLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    unitLabel->setAlignment(Qt::AlignRight);
    return unitLabel;
}

void ParameterForm::deleteLabels() {
    if (titleLabel) {
        delete titleLabel;
        titleLabel = 0;
    }
    if (valueLabel) {
        delete valueLabel;
        valueLabel = 0;
    }
    if (unitLabel) {
        delete unitLabel;
        unitLabel = 0;
    }
}*/


QString ParameterForm::getMinComm() {
    return minComm;
}

QString ParameterForm::getMaxComm() {
    return maxComm;
}

QString ParameterForm::getRealComm() {
    return realComm;
}

QString ParameterForm::getValueComm() {
    return valueComm;
}

void ParameterForm::setMinComm(QString str) {
    minComm = str;
}

void ParameterForm::setMaxComm(QString str) {
    maxComm = str;
}

void ParameterForm::setRealComm(QString str) {
    realComm = str;
}

void ParameterForm::setValueComm(QString str) {
    valueComm = str;
}

bool ParameterForm::isTemperature() {
    return isTemperatureFlag;
}
