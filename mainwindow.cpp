#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupWindow();
    setConnections();
}

MainWindow::~MainWindow()
{
    settings.setLastSelectedDeviceId(0);
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event) {
//    if(maybeSave()) {
        saveWindowSettings();
        // Остановка источника (реализуй)
        if(portIsOpen && isDeviceLoaded) {
            sendDataToPort(DEVICE_STOP_COMMAND);
            serialPort->close();

        }
        event->accept();
//    } else {
//        event->ignore();
//    }
}

//void MainWindow::resizeEvent(QResizeEvent *event) {
//    this->updateWindow();
//}


void MainWindow::setConnections() {
    // Запоминаем последние 10 отправленных команд
    //connect(this, SIGNAL(sendToPort(QString)), this, SLOT(setLastSentCommand(QString)));

    // Нажатие кнопки соединения\отключения COM-порта
    connect(ui->comPortConnectButton, SIGNAL(released()), this, SLOT(comPortConnectButton_slot()));
    // Отправка сообщений введённых в консоли по нажатию Enter
    connect(ui->comPortConsoleTextEdit, SIGNAL(returnPressed()), this, SLOT(comPortConsoleSend_Slot()));


    // Связи для меню File
    // Вызов окна открытия файла настроек при клике в меню file->load settings
    connect(ui->actionLoad_settings, SIGNAL(triggered(bool)), this, SLOT(loadSettingsSlot()));
    // Сохранение параметров для меня file->save settings
    connect(ui->actionSave_settings, SIGNAL(triggered(bool)), this, SLOT(saveSettingsSlot()));
    // Выход из программы
    connect(ui->actionExit, SIGNAL(triggered(bool)), QApplication::instance(), SLOT(quit()));

    // Выбор количества стоп-бит
    connect(ui->actionOne_stop_bit, SIGNAL(toggled(bool)), this, SLOT(setComOneStopBit()));
    connect(ui->actionTwo_stop_bits, SIGNAL(toggled(bool)), this, SLOT(setComTwoStopBits()));
    // Действие для меню Autoconnect
    connect(ui->actionCompact_view, SIGNAL(toggled(bool)), this, SLOT(toogleCompactModeSlot(bool)));

    // Связи для меню Help
    // Вызов окна "about" через меню
    connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(showAboutDialogSlot()));
    // Вызов справки через меню
    connect(ui->actionHelp, SIGNAL(triggered(bool)), this, SLOT(openHelpSlot()));
    // Вызов действия проверки обновлений через меню
    connect(ui->actionCheck4updates, SIGNAL(triggered(bool)), this, SLOT(checkForUpdatesSlot()));
    // Вызов окна "отправить лог" через меню
    connect(ui->actionSendLog, SIGNAL(triggered(bool)), this, SLOT(sendLogSlot()));

    // Кнопка "hide contrtols"
    connect(ui->controlsVisibleButton, SIGNAL(clicked(bool)), this, SLOT(hideControlsButtonSlot(bool)));
    // Связь сигнала и слота для меню "autoconnect"
    connect(ui->actionAutoconnect, SIGNAL(triggered(bool)), this, SLOT(triggComAutoConnectSlot(bool)));

    // Отправка новых значений пределов параметров в устройство
    connect(changeLimitsDialog, SIGNAL(sendData(QString)), this, SLOT(sendDataToPort(QString)));
    // Отправка новых значений калибровочных коэффициентов параметров в устройство
    connect(calibrateDialog, SIGNAL(sendData(QString)), this, SLOT(sendDataToPort(QString)));

    // Выход из админ-режима при нажатии соответствующей кнопки
    connect(ui->hideConsoleButton, SIGNAL(clicked(bool)), this, SLOT(showConsoleSlot(bool)));

    /*
     * com port connections
     */
    // Связка полученных данных от ком-порта и функции чтения этих данных
    QObject::connect(serialPort, SIGNAL(receivedDataSignal(QByteArray)), this, SLOT(readComData_Slot(QByteArray)));
    // Запись отправленной команды в лог и вывод в консоль
    QObject::connect(serialPort, SIGNAL(writeToConsoleSignal(QString, Qt::GlobalColor)), this, SLOT(writeToConsole(QString,Qt::GlobalColor)));
    // Если посылка установки параметра успешно отправлена и не требует обработки ответа устройства
    QObject::connect(serialPort, SIGNAL(dataIsSentSignal()), this, SLOT(prepareToSendNextCommand()));
    // Установка флага состояния порта в главном окне, при изменении состояния порта
    QObject::connect(serialPort, SIGNAL(portNewState(bool)), this, SLOT(getPortNewState(bool)));
    // Изменение состояние COM-порта при нажатии кнопки connect\disconnect
//    QObject::connect(this, SIGNAL(setPortNewState(bool)), serialPort, SLOT(setPortState(bool)));
    // Отправка данных в ком порт из главного окна программы
//    QObject::connect(this, SIGNAL(sendToPort(QString)), serialPort, SLOT(sendData(QString)));
    // Если произошел таймаут СОМ-порта - сообщаем в главное окно
    QObject::connect(serialPort, SIGNAL(portTimeroutOccure()), this, SLOT(comPortTimeout()));
    // Произошла ошибка COM-порта - пишем в лог и консоль
    QObject::connect(serialPort, SIGNAL(errorOccuredSignal(QString)), this, SLOT(comPortError(QString)));
    // Разрешение или запрет циклической отсылки комманд в ком-порт
    //QObject::connect(this, SIGNAL(transmitCycleEnable(bool)), serialPort, SLOT(enableCycleTransmit(bool)));

    // Если установленно "автоподключение", то устанавливаем связь с ком-портом при запуске программы
    if(settings.getComAutoconnectFlag()) serialPort->setPortState(true);

    // Обработка нажатия кнопки начала\остановки авторассылки команд
    connect(ui->consoleStartStopButton, SIGNAL(clicked(bool)), this, SLOT(comSetDataTransfer(bool)));
    // Кнопка посыла следующей команды при паузе авторассылки команд
    connect(ui->consoleNextCommandButton, SIGNAL(clicked(bool)), this, SLOT(sendNextComCommand()));

    // Подключение слотов-обработчиков нажатия кнопок LASER и TEC
    connect(ui->laserButton, SIGNAL(clicked(bool)), this, SLOT(laserButtonSlot()));
    connect(ui->tecButton, SIGNAL(clicked(bool)), this, SLOT(tecButtonSlot()));

    // Настройка и создание экземпляра окна "о программе"
    //connect(aboutDialog, SIGNAL(finished(int)), aboutDialog, SLOT(deleteLater()));
    connect(aboutDialog, SIGNAL(startUpdate()), this, SLOT(startUpdateSlot()));
    connect(aboutDialog, SIGNAL(showConsoleSignal(bool)), this, SLOT(showConsoleSlot(bool)));

    // Обработчик нажатия кнопки device options
    connect(ui->devAboutButton, SIGNAL(clicked(bool)), aboutDeviceDialog, SLOT(show ()));
    // Обработчик нажатия кнопки Select device для устройств общего id
    connect(ui->devSelectButton, SIGNAL(clicked(bool)), this, SLOT(reselectDevice()));

    // Сигналы логгера
    // Запись событий главного окна в лог
    //QObject::connect(this, SIGNAL(writeToLogSignal(QString)), logger, SLOT(writeToLog(QString)));
    // Отправка ошибок логгера в консоль основного окна
    QObject::connect(logger, SIGNAL(errorOccuredSignal(QString)), this, SLOT(writeToConsoleError(QString)));
    logger->start();

    // Отправка модели выбранного драйвера (для изделий со старым айди) в главную программу
    connect(selectDeviceDialog, SIGNAL(selectedDevice(QString)), this, SLOT(selectedDeviceSlot(QString)));
}

void MainWindow::setupWindow() {
    loadFont();
    setWindowTitle(appTitle);
    setWindowIcon(QIcon(":/images/logo-minimal.png"));

    loadWindowSettings();

    // Временно скрываем меню help
    ui->actionHelp->setVisible(false);

    // Определение вспомогательных классов
    logger = new consoleLogger();
    serialPort = new comPort();
    changeLimitsDialog = new ChangeLimitsDialog(this);
    calibrateDialog = new CalibrateDialog(this);
    xml = nullptr;
    actualParamsGLayout = new QGridLayout();
    aboutDeviceDialog = new AboutDeviceDialog();
    bitsLayout = new BitsLayout(ui->bitMaskBox);
    ui->bitMaskBox->setLayout(bitsLayout->getLayout());
    selectDeviceDialog = new SelectDeviceDialog();

    currCommandItt = nullptr;
    oldDevID = 0;
    autoSendNextCommand = true;
    requestAllCommands = true;

    loadProgramConfig(availableDevices);

    ui->comPortConnectButton->setText(tr("Disconnected"));
    portIsOpen = false;
    isDeviceLoaded = false;
    //setLink(portIsOpen);

    ui->actionAutoconnect->setChecked(settings.getComAutoconnectFlag());
    ui->actionSave_settings->setEnabled(false);
    ui->actionLoad_settings->setEnabled(false);
    ui->controlsVisibleButton->setChecked(false); //settings.getHideControlsFlag());
    ui->controlsVisibleButton->setText("Hide controls");

    setupMenuView();
    setupMenuPort();
    refreshMenuFile();
    ui->actionExit->setShortcut(QKeySequence("ALT+F4"));
    //checkForUpdatesSlot();

//    ui->devImageLabel->setVisible(false);
    ui->devNameLabel->setVisible(false);
    ui->devAboutButton->setVisible(false);
    ui->devSelectButton->setVisible(false);

    ui->comPortConsole->setMaximumBlockCount(CONSOLE_MAX_LINES_TO_SHOW);

    // Настройка и создание экземпляра окна "о программе"
    aboutDialog = new AboutDialog();

    ui->tecButton->setVisible(false);
    ui->laserButton->setVisible(false);

    comPortIntervalCounter = 1;

#ifndef DEBUG_MODE
    showConsoleSlot(false);
#endif
}

//void MainWindow::setupIndicators() {
//    bitsLayout = new BitsLayout(this);
//    //ui->bitMaskBox->setLayout(bitsLayout->getLayout());
//}

void MainWindow::setupMenuPort() {
//    refreshMenuPort();
//    connect(ui->menuSelectPort, SIGNAL(aboutToShow()), this, SLOT(refreshMenuPort()));

    // Формирование и вывод в меню списка доступных бауд-рейтов
    ui->menuSelectBaudrate->clear();
    QSignalMapper* signalMapper = new QSignalMapper(this);
    foreach (quint32 BR, comBaudRates) {
        QAction *newAct = new QAction(QString::number(BR), this);
        newAct->setCheckable(true);
        if(settings.getComBaudrate() == BR) {
            newAct->setChecked(true);
        } else {
            newAct->setChecked(false);
        }
        ui->menuSelectBaudrate->addAction(newAct);
        signalMapper->setMapping(newAct, BR);
        connect(newAct, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
    }
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(changeBaudRateSlot(int)));

    refreshMenuPort();
    refreshMenuBaud();
    refreshMenuStopBits();
}

void MainWindow::refreshMenuPort() {
    // Формирование и вывод в меню списка доступных портов
//    ui->menuSelectPort->clear();
    foreach(QAction* item, ui->menuSelectPort->actions()) {
        if(item->text() == tr("Refresh list")) {
            ui->menuSelectPort->removeAction(item);
            delete item;
        }
    }


    QStringList portList = getAvailablePorts();
    if(!portList.isEmpty()) {
        ui->menuSelectPort->clear();
        QSignalMapper* signalMapper = new QSignalMapper(this);
        foreach (QString str, portList) {
            QAction *newAct = new QAction(str, this);
            newAct->setCheckable(true);
            if(QString::compare(settings.getComPort(), str, Qt::CaseInsensitive) == 0) {
                newAct->setChecked(true);
            } else {
                newAct->setChecked(false);
            }
            ui->menuSelectPort->addAction(newAct);
            signalMapper->setMapping(newAct, str);
            connect(newAct, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
        }
        connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(changePortSlot(QString)));
    }
    // кнопка обновления портов
    QAction *refreshPortAct = new QAction(tr("Refresh list"), this);
    ui->menuSelectPort->addSeparator();
    ui->menuSelectPort->addAction(refreshPortAct);
    connect(refreshPortAct, SIGNAL(triggered(bool)), this, SLOT(refreshMenuPort()));

    foreach (QAction *act, ui->menuSelectPort->actions()) {
        if(QString::compare(settings.getComPort(), act->text(), Qt::CaseInsensitive) == 0) {
            act->setChecked(true);
        } else {
            act->setChecked(false);
        }
    }
}

void MainWindow::refreshMenuBaud() {
    foreach (QAction *act, ui->menuSelectBaudrate->actions()) {
        if(settings.getComBaudrate() == act->text().toUInt()) {
            act->setChecked(true);
        } else {
            act->setChecked(false);
        }
    }
}

void MainWindow::refreshMenuStopBits() {
    foreach (QAction *act, ui->menuStop_bits->actions()) {
        if(act->text() == "One stop bit") {
            if(settings.getComStopBits() == 1) {
                act->setChecked(true);
            } else {
                act->setChecked(false);
            }
        } else if(act->text() == "Two stop bits") {
            if(settings.getComStopBits() == 2) {
                act->setChecked(true);
            } else {
                act->setChecked(false);
            }
        }
    }
}

void MainWindow::refreshMenuCalibrate() {
    if(devConfig.calibrate.isEmpty()) {
        ui->menuCalibrate->menuAction()->setVisible(false);
        return;
    } else {
        ui->menuCalibrate->menuAction()->setVisible(true);
    }

    QSignalMapper* signalMapper = new QSignalMapper(this);

    if(!menuCalibrateActions.isEmpty()) {
        ui->menuCalibrate->clear();
        menuCalibrateActions.clear();
    }

    ui->menuCalibrate->setEnabled(true);

    foreach(calibration_t item, devConfig.calibrate) {
        QAction *newAction = new QAction();
        newAction->setText(item.title);
        ui->menuCalibrate->addAction(newAction);
        menuCalibrateActions.append(newAction);
        signalMapper->setMapping(newAction, item.title);
        connect(newAction, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
    }
    connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(openCalibrateWindow(QString)));
}


void MainWindow::openCalibrateWindow(QString name) {
    for(uint8_t i = 0; i < devConfig.calibrate.size(); i++) {
        const calibration_t& item = devConfig.calibrate.at(i);
        if(item.title == name) {
            calibrateDialog->setStruct(item);
            foreach(parameter_t* param, devConfig.commands) {
                if(param->getCode() == item.code) {
                    calibrateDialog->setValue(param->getRawValue());
                }
            }
            calibrateDialog->show();
            break;
        }
    }
}

void MainWindow::refreshMenuLimits() {
    if(devConfig.limits.isEmpty()) {
        ui->menuLimits->menuAction()->setVisible(false);
        return;
    } else {
        ui->menuLimits->menuAction()->setVisible(true);
    }
    QSignalMapper* signalMapper = new QSignalMapper(this);

    if(!menuLimitsActions.isEmpty()) {
        ui->menuLimits->clear();
        menuLimitsActions.clear();
    }

    ui->menuLimits->setEnabled(true);

    foreach(limits_t item, devConfig.limits) {
        QAction *newAction = new QAction();
        newAction->setText(item.title);
        ui->menuLimits->addAction(newAction);
        menuLimitsActions.append(newAction);
        signalMapper->setMapping(newAction, item.title);
        connect(newAction, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
    }
    connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(openLimitsWindow(QString)));
}

void MainWindow::openLimitsWindow(QString name) {
    for(uint8_t i = 0; i < devConfig.limits.size(); i++) {
        const limits_t& item = devConfig.limits.at(i);
        if(item.title == name) {
            changeLimitsDialog->setStruct(item);
            foreach(parameter_t* param, devConfig.commands) {
                if(param->getCode() == item.absMaxCode) {
                    changeLimitsDialog->setAbsMax(param->getConvertedValue());
                } else if(param->getCode() == item.absMinCode) {
                    changeLimitsDialog->setAbsMin(param->getConvertedValue());
                } else if(param->getCode() == item.maxCode) {
                    changeLimitsDialog->setMax(param->getConvertedValue());
                } else if(param->getCode() == item.minCode) {
                    changeLimitsDialog->setMin(param->getConvertedValue());
                }
            }

            changeLimitsDialog->show();
            break;
        }
    }
}

void MainWindow::refreshMenuFile() {
    if(!lastFileActions.isEmpty()) {
        foreach (QAction* item, lastFileActions) {
            item->deleteLater();
        }
    }
    lastFileActions.clear();

    QSignalMapper* signalMapper = new QSignalMapper(this);
    ui->menuFile->addSeparator();
    foreach(QVariant listVar, settings.getRecentOpenFiles()) {
        QString fileName = listVar.toString();
        // Проверка существует ли файл.
        if(!QFile::exists(fileName)) {
            settings.removeRecentOpenFiles(fileName);
            continue;
        }

        QAction *newAct = new QAction(fileName, ui->menuFile);
        signalMapper->setMapping(newAct, fileName);
        connect(newAct, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
        newAct->setEnabled(link);
        lastFileActions.prepend(newAct);
        ui->menuFile->addAction(newAct);
    }
    connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(readSettingsFile(QString)));
}



void MainWindow::readSettingsFile(QString fileName) {
    if(!QFile::exists(fileName)) {
        settings.removeRecentOpenFiles(fileName);
        refreshMenuFile();
        writeToConsoleError("Settings file \"" + fileName + "\" is not exist.");
        QMessageBox::warning(this, tr("Settings file wasn't loaded"), "Settings file \"" + fileName + "\" is not exist.", QMessageBox::Ok);
        return;
    }

    QFile *file = new QFile(fileName);
    file->open(QIODevice::ReadOnly | QIODevice::Text);
    if(file->error()) {
        writeToConsoleError(file->errorString());
        //emit writeToLogSignal(file->errorString());
    }
    bool autoSend = autoSendNextCommand;
    autoSendNextCommand = false;

    QTextStream in(file);
    while(!in.atEnd()) {
        QStringList parse = in.readLine().split(":", QString::SkipEmptyParts, Qt::CaseInsensitive);
        if(!parse.isEmpty() && parse.size() == 2) {
            // загрузка данных, опиши ниже
            bool deviceIsValid = false;
            if (parse.at(0).toUpper() == "DEVICE") {
                int id = parse.at(1).toInt(nullptr, 16);
                if (devID == id) deviceIsValid = true;
                if(!deviceIsValid) {
                    writeToConsoleError("This configuration file was created for another device");
                    QMessageBox::warning(this, tr("Config file wasn't loaded"), tr("This configuration file was created for another device"), QMessageBox::Ok);
                    return;
                }
                writeToConsole("Load config file \"" + fileName + "\" for devID: " + QString::number(devID));
            } else {
                QString msgForSend = COM_WRITE_PREFIX + parse.at(0) + QString(" ") + parse.at(1);
//                sendToPort(msgForSend);
//                writeToConsole(msgForSend, CONSOLE_INFO_COLOR);
                sendDataToPort(msgForSend);
            }
        }
    }

    autoSendNextCommand = autoSend;

    delete file;
}

void MainWindow::loadSettingsSlot() {
    QString lastDir = settings.getLastSaveDirectory();
    QDir dir = QDir(lastDir);
    if(!dir.exists()) lastDir = QDir::homePath();

    QFileDialog *fd = new QFileDialog();
    fd->setDirectory(lastDir);
    fd->setFileMode(QFileDialog::ExistingFile);
    QString filename = fd->getOpenFileName(this, tr("Select file"), settings.getLastSaveDirectory(), tr("Settings files (*.msf)"));
    if(!filename.isEmpty()) {
        settings.setLastSaveDirectory(fd->directory().absolutePath());
        readSettingsFile(filename);
    }
}


void MainWindow::saveSettingsSlot() {
    if(!isDeviceLoaded) return;

    QStringList dataOut;

    QString lastDir = settings.getLastSaveDirectory();
    QDir dir = QDir(lastDir);
    if(!dir.exists()) lastDir = QDir::homePath();

    QFileDialog *fd = new QFileDialog();
    fd->setFileMode(QFileDialog::AnyFile);
    fd->setDirectory(lastDir);
    QString filename = fd->getSaveFileName(this, tr("Select file"), settings.getLastSaveDirectory(), tr("Settings files (*.msf)"));
    if(!filename.isEmpty()) {
        settings.setLastSaveDirectory(fd->directory().absolutePath());
        if(!filename.endsWith(".msf")) filename.append(".msf");
        QFile *file = new QFile(filename);
        file->open(QIODevice::WriteOnly | QIODevice::Text);
        if(file->error()) {
            writeToConsoleError(file->errorString());
            //emit writeToLogSignal(file->errorString());
        }

        QTextStream dataOut(file);

        // Процесс записи данных в файл
        bool tmpAutoSend = autoSendNextCommand;
        autoSendNextCommand = false;

        QString currentLine = QString("DEVICE:%1").arg(devID, 4, 16, QChar('0'));
        dataOut << currentLine.toUpper() << endl;

        foreach(parameter_t* commPtr, devConfig.commands) {
            currentLine.clear();
            currentLine = QString("%1:%2").arg((*commPtr).getCode()).arg((*commPtr).getRawValue(), 4, 16, QChar('0'));
            dataOut << currentLine.toUpper() << endl;
        }
        file->close();

        autoSendNextCommand = tmpAutoSend;
        // Процесс записи окончен
        QList<QVariant> tmpList = settings.getRecentOpenFiles();
        if(tmpList.contains(filename)) {
            tmpList.removeAll(filename);
            settings.removeRecentOpenFiles(filename);
        }
        if(tmpList.size() >= MAX_FILES_IN_MENU) tmpList.removeLast();
        tmpList.prepend(filename);
        settings.setRecentOpenFiles(tmpList);

        refreshMenuFile();

        file->deleteLater();
    }
}

void MainWindow::setupMenuView() {
    QSignalMapper* signalMapper = new QSignalMapper(this);
    signalMapper->setMapping(ui->actionTemperature_in_C, "C");
    signalMapper->setMapping(ui->actionTemperature_in_F, "F");
    connect(ui->actionTemperature_in_C, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
    connect(ui->actionTemperature_in_F, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
    connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(triggTemperatureSymbolSlot(QString)));

    refreshMenuView();
}

void MainWindow::refreshMenuView() {
    if(QString::compare(settings.getTemperatureSymbol(), "C", Qt::CaseInsensitive) == 0) {
        ui->actionTemperature_in_C->setChecked(true);
        ui->actionTemperature_in_F->setChecked(false);
    } else {
        ui->actionTemperature_in_C->setChecked(false);
        ui->actionTemperature_in_F->setChecked(true);
    }
    ui->actionCompact_view->setChecked(settings.getCompactModeFlag());
}

void MainWindow::setupParameterHandlers() {

    serialPort->setStopCommandDelay(devConfig.stopCommandDelay);

    if(!devConfig.paramWidgets.empty()) {
//        QHBoxLayout* hlayoutparams = new QHBoxLayout();
//        QVBoxLayout* vlayoutparams = new QVBoxLayout();
        QGridLayout* layoutparams = new QGridLayout();

        ui->controlsVisibleButton->setChecked(false); //settings.getHideControlsFlag());
        ui->controlsVisibleButton->setText("Hide controls");
        aboutDeviceDialog->setInfo(devConfig);

        refreshMenuLimits();
        refreshMenuCalibrate();
//        ui->bitMaskBox->setLayout(bitsLayout->getLayout());

        bitsLayout->addFromList(devConfig.leds);

        int rowcol = 0;
        int row = 0;
        for (QList<ParameterForm*>::iterator itemPtr = devConfig.paramWidgets.begin(); itemPtr < devConfig.paramWidgets.end(); itemPtr++) {
            (*itemPtr)->setEnableState(true);
            connect((*itemPtr), SIGNAL(changeValue(QString)), this, SLOT(sendDataToPort(QString)));
//            if((*itemPtr)->isTemperature() && settings.getTemperatureSymbol() == 'F') {
//            if((*itemPtr)->isTemperature()) {
//                (*itemPtr)->setUnit(QString::fromRawData(new QChar('\260'), 1) + settings.getTemperatureSymbol());
//            }
            (*itemPtr)->setRealValue(0); // default

            actualParamsGLayout->addWidget((*itemPtr)->loadTextWidget(), row, 0);
            row++;

            if(!(*itemPtr)->getValueComm().isEmpty()) { // Вывод "крутилок-таскалок" для параметров
                (*itemPtr)->setMin(0); // default
                (*itemPtr)->setMax(100); // default
                (*itemPtr)->setSentValue(25); // default
                // Сокрытие\показ дополнительных полей если параметр доступен только для записи или для чтения\записи
                (*itemPtr)->hideRealValue((*itemPtr)->getRealComm().isEmpty());

                if(settings.getCompactModeFlag()) {
                    (*itemPtr)->loadCompactWidget()->setVisible(true);
                    (*itemPtr)->loadWidget()->setVisible(false);
                } else {
                    (*itemPtr)->loadCompactWidget()->setVisible(false);
                    (*itemPtr)->loadWidget()->setVisible(true);
                }

                layoutparams->addWidget((*itemPtr)->loadWidget(), 0, rowcol);
                layoutparams->addWidget((*itemPtr)->loadCompactWidget(), rowcol+1, 0);
                (*itemPtr)->loadTextWidget()->setVisible(false);
                rowcol++;
            } else { // Вывод значений в окно параметров
//                actualParamsGLayout->removeWidget((*itemPtr)->loadTextWidget());
//                actualParamsGLayout->addWidget((*itemPtr)->loadTextWidget(), row, 0);
//                row++;
                (*itemPtr)->loadTextWidget()->setVisible(true);
            }
        }

        // Крутилки-вертелки для параметром
        if(ui->parametersGroupBox->layout()) delete ui->parametersGroupBox->layout();
        ui->parametersGroupBox->setLayout(layoutparams);
        // Вывод значений в окне параметров
        ui->actualParameters->setLayout(actualParamsGLayout);

        // Вынеси в commondefines список параметров
        // Формирование и вывод на экран панели режимов
        QSignalMapper* cbSignalMapper = new QSignalMapper(this);
        if(!devConfig.specialParameters.empty()) {
            QVBoxLayout *vlayout = new QVBoxLayout();
            for(int i = 0; i < devConfig.specialParameters.count(); i++) {
                QCheckBox *tmpCheckBox = devConfig.specialParameters.at(i).cbPtr;
                tmpCheckBox->setStyleSheet("QCheckBox {font-family: \"Share Tech Mono\"; border: none; color: #fff;} ");
                vlayout->addWidget(tmpCheckBox);
                connect(tmpCheckBox, SIGNAL(clicked(bool)), cbSignalMapper, SLOT(map()));
                cbSignalMapper->setMapping(tmpCheckBox, devConfig.specialParameters.at(i).label);
            }
            connect(cbSignalMapper, SIGNAL(mapped(QString)), this, SLOT(spcialParameterSlot(QString)));
            ui->specialParamBox->setLayout(vlayout);
        }
    }

    // Вывод информации о выбранном устройстве
    ui->devNameLabel->setText(devConfig.devName);
    ui->devNameLabel->setVisible(true);
    ui->devAboutButton->setVisible(true);

    // Вывод на экран или сокрытие кнопок "Laser" и "tec"
    ui->laserButton->setVisible(devConfig.hasLaser);
    ui->tecButton->setVisible(devConfig.hasTEC);

//    updateWindow();
}

void MainWindow::clearAllRegulators() {

    for(QList<ParameterForm*>::ConstIterator i = devConfig.paramWidgets.constBegin(); i != devConfig.paramWidgets.constEnd(); i++) {
        delete (*i)->loadWidget();
        delete (*i)->loadCompactWidget();
        delete (*i)->loadTextWidget();
        delete (*i);
    }
    devConfig.paramWidgets.clear();

    bitsLayout->clear();

    foreach(parameter_t* itemPtr, devConfig.commands) {
        delete itemPtr;
    }


    foreach(specParams_t item, devConfig.specialParameters) {
         delete item.cbPtr;
    }


    devConfig.limits.clear();
    devConfig.calibrate.clear();
    devConfig.commands.clear();
    devConfig.description = "";
    devConfig.devName = "";
    devConfig.hasLaser = false;
    devConfig.hasTEC = false;
    devConfig.image = "";
    devConfig.link = "";
    devConfig.paramWidgets.clear();
    devConfig.specialParameters.clear();
    devConfig.stateButtons.clear();
    devConfig.leds.clear();

    if(ui->parametersGroupBox->layout()) delete ui->parametersGroupBox->layout();
//    if(ui->bitMaskBox->layout()) delete ui->bitMaskBox->layout();
    bitsLayout->clear();
    if(ui->specialParamBox->layout()) delete ui->specialParamBox->layout();
//    if(ui->actualParameters->layout()) delete actualParamsGLayout;

    this->updateWindow();
}

void MainWindow::setRegulatorsEnable(bool state) {
    for(qint8 i = 0; i < devConfig.paramWidgets.count(); i++) {
        ParameterForm *item = devConfig.paramWidgets.at(i);
        item->setEnableState(state);
    }

    for(qint8 i = 0; i < devConfig.specialParameters.count(); i++) {
        devConfig.specialParameters.at(i).cbPtr->setEnabled(state);
    }

    comSetDataTransfer(state);
    if(state) {
        requestAllCommands = true;
    }
}

void MainWindow::getPortNewState(bool state) {
    if (state != portIsOpen) {
        if(state) { // Port is openned now
            this->writeToConsole(settings.getComPort() + tr(" is open!\n"), CONSOLE_INFO_COLOR);
            //emit writeToLogSignal(tr("COM: ") + COM_CURRENT_PORT + " (" + QString::number(COM_CURRENT_BAUDRATE) + "bps)" + tr(" is open!\n"));
            ui->comPortConnectButton->setText(settings.getComPort() + QString("   ") + QString::number(settings.getComBaudrate()) + "bps");
            startDeviceIdent();
            setLink(true);
            if(ui->statusBar->currentMessage().size() > 0) ui->statusBar->showMessage("");
        } else { // Port is closed now
            this->writeToConsole(settings.getComPort() + tr(" is close!\n"), CONSOLE_INFO_COLOR);
            //emit writeToLogSignal(tr("COM: ") + COM_CURRENT_PORT + tr(" is close!\n"));
            ui->comPortConnectButton->setText(tr("Disconnected"));
//            if(devID == COMMON_OLD_DEVICES_VALUE) isDeviceLoaded = false;
            devID = 0;
            settings.setLastSelectedDeviceId(0);
            setLink(false);
            setRegulatorsEnable(false);
//            ui->menuLimits->setEnabled(false);
        }
        ui->consoleStartStopButton->setEnabled(state);
        ui->consoleStartStopButton->setChecked(!autoSendNextCommand);
    }

    portIsOpen = state;
    ui->comPortConnectButton->setChecked(!portIsOpen);

    ui->actionLoad_settings->setEnabled(portIsOpen);
    foreach (QAction *item, lastFileActions) {
        item->setEnabled(portIsOpen);
    }


    if(!(isDeviceLoaded && state == false))
        ui->actionSave_settings->setEnabled(portIsOpen);
}

void MainWindow::startDeviceIdent() {
    devID = 0;
    comSetDataTransfer(false);

    requestAllCommands = true;

    currCommandItt = nullptr;
    if (!devConfig.commands.isEmpty()) {
        currCommandItt = devConfig.commands.constBegin();
        comPortIntervalCounter = 1;
    }
    QString tmpStr = QString("%1%2").arg(COM_REQUEST_PREFIX).arg(IDENTIFY_DEVICE_COMMAND, 4, 16, QChar('0'));
    sendDataToPort(tmpStr);
}

void MainWindow::comPortConnectButton_slot()
{
    serialPort->setPortState(!portIsOpen);
}

void MainWindow::comPortConsoleSend_Slot() {
    QString str = ui->comPortConsoleTextEdit->text();
    ui->comPortConsoleTextEdit->clear();
    sendDataToPort(str);
}

void MainWindow::readComData_Slot(QByteArray str) {
    //emit writeToLogSignal("COM <- :" + str);
    quint16 command = 0;
    // qint16
    quint16 value = 0;
    char prefix = '0';

    setLink(true);

    QString commandStr, valueStr;
    commandStr = str.mid(1, 4);
    valueStr = str.mid(6, 4);

    prefix = str.at(0);
    command = commandStr.toUInt(nullptr, 16);
    value = valueStr.toUInt(nullptr, 16);

    // Проверка первого символа в посылке
    if(prefix == COM_ANSWER_PREFIX) { // Если это ответ
        this->writeToConsole("<- "+str, CONSOLE_RECEIVE_COLOR);
        if(command == IDENTIFY_DEVICE_COMMAND) {
            if(value == COMMON_OLD_DEVICES_VALUE) {
                devID = settings.getLastSelectedDeviceId();
                bool isCommonDevice = false;
                foreach(availableDev_t devStruct, availableDevices) {
                    if(devStruct.id == devID) {
                        isCommonDevice = true;
                        break;
                    }
                }

                if(!isCommonDevice) {
                    writeToConsoleError("Device id (" + QString::number(devID) + ") is not represented in config file.");
                    writeToConsole("Try to select the deivce manually.");
                    reselectDevice();
                }

                ui->devSelectButton->setVisible(true);
                ui->devAboutButton->setVisible(true);
//                aboutDeviceDialog->setInfo(devConfig);
            } else {
                devID = value;
            }

            if(!isDeviceLoaded) {
                clearAllRegulators();
                loadConfig(devID);
            } else {
                // Если это то же устройство, что и было ранее, то
                if(devID == oldDevID) {
                    setRegulatorsEnable(true);
                } else {
                    clearAllRegulators();
                    loadConfig(devID);
                }
            }
            ui->devNameLabel->setText(devConfig.devName);
            oldDevID = devID;
        } else {
            bool isValidCommand = false;
            QList<parameter_t*>::iterator paramPtr;
            for(paramPtr = devConfig.commands.begin(); paramPtr != devConfig.commands.end(); paramPtr++) {
                if((*paramPtr)->getCode().compare(commandStr, Qt::CaseInsensitive) == 0) {
                    isValidCommand = true;
                    (*paramPtr)->setRawValue(value);
                    break;
                }
            }

            if (isValidCommand) {
                // Обработка крутилок и информеров основных параметров
                for(QList<ParameterForm*>::iterator pwPtr = devConfig.paramWidgets.begin(); pwPtr != devConfig.paramWidgets.end(); pwPtr++) {
                    double newValue;
                    // Если это температура, то конвертируем значение в "знаковое"
                    if((*pwPtr)->isTemperature()) {
                        newValue = (*paramPtr)->getConvertedSignedValue();
                    } else {
                        newValue  = (*paramPtr)->getConvertedValue();
                    }


                    if((*pwPtr)->isTemperature() && settings.getTemperatureSymbol() == "F") {
                        newValue = convertCelToFar(newValue);
                    }

                    if((*pwPtr)->getMaxComm().compare(commandStr, Qt::CaseInsensitive) == 0) {
                        (*pwPtr)->setMax(newValue);
                    } else if((*pwPtr)->getMinComm().compare(commandStr, Qt::CaseInsensitive) == 0) {
                        (*pwPtr)->setMin(newValue);
                    } else if((*pwPtr)->getRealComm().compare(commandStr, Qt::CaseInsensitive) == 0) {
                        (*pwPtr)->setRealValue(newValue);
                    } else if((*pwPtr)->getValueComm().compare(commandStr, Qt::CaseInsensitive) == 0) {
                        (*pwPtr)->setSentValue(newValue);
                    }
                }

                // Обработка LEDS
                setLedState(commandStr, value);

                // Обработка галочек
                for(int i = 0; i < devConfig.specialParameters.size(); i++) {
                    specParams_t obj = devConfig.specialParameters.at(i);
                    if(obj.mask == 0) continue;

                    if(commandStr == obj.code) {
                        if(value & obj.mask) {
                            obj.cbPtr->setChecked(true);
                        } else {
                            obj.cbPtr->setChecked(false);
                        }
                    }
                }

                // Обработка команды 0700
                if(commandStr == DEVICE_STATUS_COMMAND && isValidCommand) {
                    quint16 newValue = (*paramPtr)->getRawValue();
                    if(newValue & START_STOP_MASK) {
                        if(devConfig.hasLaser) {
                            ui->laserButton->setChecked(true);
                        }
                    } else {
                        if(devConfig.hasLaser) {
                            ui->laserButton->setChecked(false);
                        }
                    }


                }
            } // isValidCommand

        }
        //    } else if(str.at(0) == COM_ERROR_PREFIX) { // Если это ошибка
        //        this->writeToConsole(str, Qt::red);
//    } else if(preifx == COM_WRITE_PREFIX) {
//         if(COM_WAIT_ANSWER_FOR_SET)
    } else if (prefix == COM_ERROR_PREFIX) {
        this->writeToConsoleError("<- "+str);
        switch(command) {
        case ERROR_BUFFER_OVERLOAD:
        case ERROR_WRONG_COMMAND:
            //emit writeToLogSignal(commErrorsDescription[command]);
            writeToConsoleError("[Error]:" + commErrorsDescription[command] + QString(" ") + str);
            break;
        default:
            writeToConsoleError("[Error]: " + str);
        }
        startDeviceIdent();
    } else { // Неизвестный ответ
        this->writeToConsoleError(str);
    }

    prepareToSendNextCommand();

}

void MainWindow::setLedState(QString commandStr, quint16 value) {
    QString errorMsg = "";
     foreach(leds_t led, devConfig.leds) {
        int id = bitsLayout->findElement(led.label);
        if (id >= 0) {
            bool maskEnable = false;
            bool maskCommand = false;
            ledMask_t enableMask;
            foreach(ledMask_t mask, led.masks) {
                if(mask.command == commandStr) {
                    maskCommand = true;
                    if(mask.mask & value) {
                        maskEnable = true;
                        enableMask = mask;
                        if(!mask.message.isEmpty()) {
                            if(errorMsg.isEmpty())
                                errorMsg = mask.message;
                            else
                                errorMsg += QString(" + ") + mask.message;
                        }
                    }
                }
            }

            if(maskCommand) {
                if(maskEnable) {
                    bitsLayout->setElementColor(id, enableMask.activeColor);
                    if(!errorMsg.isEmpty()) {
                        writeToConsoleError(errorMsg);
                    }
                } else {// if(!maskEnable && maskCommand) {
                    bitsLayout->setElementColor(id, enableMask.defaultColor);
                }
            } // end of "if(maskCommand)"
        } // end of "if(id >= 0)"
    }
}

//void MainWindow::reselectDevice() {
//    bool state = portIsOpen;
//    if(portIsOpen) serialPort->setPortState(false);
//    clearAllRegulators();
//    if(selectDevice()) isDeviceLoaded = false;
//    if(state) serialPort->setPortState(true);
//    startDeviceIdent();
//}

void MainWindow::reselectDevice() {
    QStringList avDevs;
    int currSelectedItem = -1;
    int i = 0;
    foreach(availableDev_t devStruct, availableDevices) {
        avDevs.append(devStruct.name);
        if(devStruct.id == devID) {
            currSelectedItem = i;
        }
        ++i;
    }
    selectDeviceDialog->setText("Select device:");
    selectDeviceDialog->setTitle("Select device dialog");
    selectDeviceDialog->setList(avDevs, currSelectedItem);
    selectDeviceDialog->open();
}

void MainWindow::selectedDeviceSlot(QString userSelectedDevice) {
        isDeviceLoaded = false;
        foreach(availableDev_t devStruct, availableDevices) {
            if(devStruct.name.compare(userSelectedDevice, Qt::CaseInsensitive) == 0) {
                bool state = false;
                if(devID != devStruct.id) {
                    state = portIsOpen;
                    if(portIsOpen) serialPort->setPortState(false);
                    clearAllRegulators();
                }
                devID = devStruct.id;
                settings.setLastSelectedDeviceId(devID);
                isDeviceLoaded = true;

                if(state) serialPort->setPortState(true);

                break;
            }
        }
}

void MainWindow::setComOneStopBit() {
    serialPort->setStopBits(1);
}

void MainWindow::setComTwoStopBits() {
    serialPort->setStopBits(2);
}

void MainWindow::prepareToSendNextCommand() {
    if(autoSendNextCommand && portIsOpen && isDeviceLoaded) {
        sendNextComCommand();
    }
}

void MainWindow::writeToConsoleError(QString str) {
    writeToConsole(str, CONSOLE_ERROR_COLOR);
     ui->statusBar->showMessage(str, STATUSBAR_MESSAGE_TIMEOUT);
}

void MainWindow::writeToConsole(QString str, Qt::GlobalColor color) {
    QColor qcolor = QColor(color);
    ui->comPortConsole->appendHtml("<span style=\"width: 100%; color: "+qcolor.name()+";\">"+str.toHtmlEscaped()+"</span>");
    logger->writeToLog(str);
}

void MainWindow::loadConfigFinished(bool isDeviceFound) {
    if(isDeviceFound) {
        isDeviceLoaded = isDeviceFound;

        setupParameterHandlers();
        writeToConsole(tr("CONFIG: Config is loaded successful!"));

        setRegulatorsEnable(true);
    } else {
        emit writeToConsoleError(tr("CONFIG: The device hasn't found!"));
    }

}

void MainWindow::loadConfigProgramFinished(bool state) {
    if(state) {
        writeToConsole("Program config is loaded success!");
    } else {
        writeToConsole("Can't load program config.");
    }
}

void MainWindow::loadConfig(quint16 id) {
//    if (devID == 0) return;

//    QThread *thread = new QThread();

    if(xml.isNull()) xml = new xmlReader();
    xml->setDeviceOptions(devConfig, id);
    xml->setConfigFile(CONFIG_FILE);
//    xml->moveToThread(thread);

    // При запуске нового потока начинаем загрузку конфига
//    connect(thread, SIGNAL(started()), xml, SLOT(startLoading()));
    // Если загрузка завершилась успешно - вызываем соответствующую процедуру
    connect(xml, SIGNAL(endOfLoadingConfig(bool)), this, SLOT(loadConfigFinished(bool)));
    // Если загрузка произошла с ошибкой, пишем в консоль и в лог ошибку
    connect(xml, SIGNAL(errorHandler(QString)), this, SLOT(writeToConsoleError(QString)));
    // Если загрузка завершилась (не зависимо от результата) завершаем поток
//    connect(xml, SIGNAL(endOfLoadingConfig(bool, QList<parameterTemp_t>)), thread, SLOT(quit()));
//    connect(xml, SIGNAL(errorHandler(QString)), thread, SLOT(quit()));
    // Если загрузка завершилась (не зависимо от результата) удаляем объект xml-загрузчика
    connect(xml, SIGNAL(endOfLoadingConfig(bool)), xml, SLOT(deleteLater()));
    connect(xml, SIGNAL(errorHandler(QString)), xml, SLOT(deleteLater()));
    // Когда поток больше не нужен удаляем и его
//    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    xml->startLoading();
//    thread->start();
}

void MainWindow::loadProgramConfig(QList<availableDev_t> &listPtr) {
    if(xml.isNull()) xml = new xmlReader();
    xml->setList(listPtr);
    xml->setBaudsList(comBaudRates);
    xml->setConfigFile(CONFIG_FILE);

    // Если загрузка завершилась успешно - вызываем соответствующую процедуру
    connect(xml, SIGNAL(endOfLoadingProgramConfig(bool)), this, SLOT(loadConfigProgramFinished(bool)));
    // Если загрузка произошла с ошибкой, пишем в консоль и в лог ошибку
    connect(xml, SIGNAL(errorHandler(QString)), this, SLOT(writeToConsoleError(QString)));
    // Если загрузка завершилась (не зависимо от результата) удаляем объект xml-загрузчика
    connect(xml, SIGNAL(endOfLoadingProgramConfig(bool)), xml, SLOT(deleteLater()));
    connect(xml, SIGNAL(errorHandler(QString)), xml, SLOT(deleteLater()));

    xml->readProgramConfig();
}

void MainWindow::comPortTimeout() {
    oldDevID = devID;
    setLink(false);
    setRegulatorsEnable(false);
    startDeviceIdent();

    emit writeToConsoleError(tr("The device didn't response. Com-port timeout: ") + QString::number(COM_PORT_TIMEOUT) + tr("ms"));
}

void MainWindow::comPortError(QString str) {
    oldDevID = devID;
    setLink(false);
    setRegulatorsEnable(false);
    QTimer sendTimer;
    sendTimer.singleShot(settings.getComCommandsDelay(), this, SLOT(startDeviceIdent()));
    writeToConsoleError("COM [ERROR]: " + str);
    ui->statusBar->showMessage("[ERROR]: " + str, STATUSBAR_MESSAGE_TIMEOUT);
}

void MainWindow::changeBaudRateSlot(int BR) {
    serialPort->changeBaudRate(BR);
    refreshMenuBaud();
}

void MainWindow::changePortSlot(QString port) {
    settings.setComPort(port);
    refreshMenuPort();
}

void MainWindow::triggComAutoConnectSlot(bool state) {
    settings.setComAutoconnectFlag(state);
}

void MainWindow::triggTemperatureSymbolSlot(QString str) {
    settings.setTemperatureSymbol(str);

    for(QList<ParameterForm*>::const_iterator i = devConfig.paramWidgets.constBegin(); i != devConfig.paramWidgets.constEnd(); i++) {
        if((*i)->isTemperature()) {
            (*i)->temperatureIsChanged(str);
        }
    }


    refreshMenuView();
}

void MainWindow::showAboutDialogSlot() {
    aboutDialog->show();
}

void MainWindow::openHelpSlot() {
    QDesktopServices::openUrl(QUrl(HELP_URL));
}

void MainWindow::checkForUpdatesSlot() {
    filedownloader = new FileDownloader(QUrl(UPDATE_LIST_URL) , this);
    connect(filedownloader, SIGNAL(downloaded()), this, SLOT(updateRepDownloadedSlot()));
}

void MainWindow::updateRepDownloadedSlot() {
    QString data = QString(filedownloader->downloadedData());
    if (data.isEmpty()) {
        // Оповещение об отсутствии обновлений.
        ui->statusBar->showMessage("There is no updates available.", STATUSBAR_MESSAGE_TIMEOUT);
        return;
    }
    QStringList strList = data.split(";");
    QStringList version = strList.at(0).split(".");
    QString url = strList.at(1);
    uint32_t newVer = 255*255*version.at(0).toUInt() + 255*version.at(1).toUInt() + version.at(2).toUInt();
    uint32_t currVer = MAJOR_VERSION*255*255+MINOR_VERSION*255+PATCH_VERSION;

    if (url.isEmpty()) return;

    updateUrl = url;

    if(newVer > currVer) {
        QMessageBox *msgBox = new QMessageBox();
        msgBox->setStyleSheet("QMessageBox QLabel { color: rgb(255,255,255); } QMessageBox { background-color: rgb(51,51,51); }");
        QMessageBox::StandardButton answer;
        answer = msgBox->question(this, tr("Update is available"), tr("Software update is available. Do you want to update now?"), QMessageBox::Yes|QMessageBox::No);
        if(answer == QMessageBox::Yes) {
            startUpdateSlot();
        }
        msgBox->deleteLater();
    } else {
        // Оповещение об отсутствии обновлений.
        ui->statusBar->showMessage("There is no updates available.", STATUSBAR_MESSAGE_TIMEOUT);
    }
}


void MainWindow::startUpdateSlot() {
    QDesktopServices::openUrl(QUrl(updateUrl));
}

void MainWindow::sendLogSlot() {
    QDesktopServices::openUrl(QUrl::fromLocalFile(DEFAULT_LOG_FILEPATH));
}

void MainWindow::hideControlsButtonSlot(bool state) {
    settings.setHideControlsFlag(state);
    if(state) {
        ui->controlsVisibleButton->setText(tr("Show controls"));
    } else {
        ui->controlsVisibleButton->setText(tr("Hide controls"));
    }

    for(QList<ParameterForm*>::iterator p = devConfig.paramWidgets.begin(); p != devConfig.paramWidgets.end(); p++) {
        if((*p)->getPinState() == false && !(*p)->getValueComm().isEmpty()) {
            if(settings.getCompactModeFlag()) {
                (*p)->loadCompactWidget()->setVisible(!state);
            } else {
                (*p)->loadWidget()->setVisible(!state);
            }

            if (state) { // скрыть крутилки
                if (!(*p)->getRealComm().isEmpty()) {
                    (*p)->loadTextWidget()->setVisible(true);
                }
            } else { // показать крутилки
                if (!(*p)->getRealComm().isEmpty()) {
                    (*p)->loadTextWidget()->setVisible(false);
                }
            }
        }
    }

    updateWindow();
}

void MainWindow::setLink(bool state) {
    link = state;
//    if(ui->statusBar->currentMessage().size() > 0) ui->statusBar->showMessage("");
//    refreshMenuFile();

    int linkId = bitsLayout->findElement("Link");
    if(state) {
        bitsLayout->setElementColor(linkId, Qt::green);
     } else {
        bitsLayout->setElementColor(linkId, LEDS_DEFAULT_COLOR);
        ui->devSelectButton->setVisible(false);
//        ui->devAboutButton->setVisible(false);
    }
}

void MainWindow::sendNextComCommand() {
    if(devConfig.commands.isEmpty() || !isDeviceLoaded) return;

    bool needToSend = true;
    bool cycleOn = true;

    while(cycleOn) {

        if(currCommandItt == nullptr) {
            currCommandItt = devConfig.commands.constBegin();
        }

        if(currCommandItt >= devConfig.commands.constEnd()) {
            currCommandItt = devConfig.commands.constBegin();
            comPortIntervalCounter++;
            if(comPortIntervalCounter > MAX_COM_INTERVAL_COUNTER) comPortIntervalCounter = 1;
            requestAllCommands = false;
        }

        if(!devConfig.commands.contains(*currCommandItt)) return;
        if((*currCommandItt)->getInterval() == 0)
            needToSend = false;
        else
            needToSend = ((comPortIntervalCounter % (*currCommandItt)->getInterval()) == 0);

        if(needToSend || requestAllCommands) {
            cycleOn = false;
        } else {
            currCommandItt++;
        }
    }


        QString tmpQuery = QString(COM_REQUEST_PREFIX) + (*currCommandItt)->getCode();
        sendDataToPort(tmpQuery);
        currCommandItt++;

}



void MainWindow::comSetDataTransfer(bool state) {
    serialPort->clearQueue();
    autoSendNextCommand = state;
    ui->consoleNextCommandButton->setEnabled(!state);
    if(state) {
        ui->consoleStartStopButton->setText("Stop autosend");
        prepareToSendNextCommand();
    } else {
        ui->consoleStartStopButton->setText("Start autosend");
    }
}

void MainWindow::showConsoleSlot(bool state) {
    ui->hideConsoleButton->setChecked(~state);
    ui->consoleGroupBox->setVisible(state);

    updateWindow();
}

void MainWindow::laserButtonSlot() {
    if(!portIsOpen) return;

    QMap<QString, doubleMaskCommand_t>::iterator itt;
    itt = devConfig.stateButtons.find("laser");
    if(itt == devConfig.stateButtons.end()) return;

    QString tmpQuery = QString(COM_WRITE_PREFIX) + (*itt).code + QString(" ");
    if(ui->laserButton->isChecked()) {
        tmpQuery.append((*itt).onCommand);
    } else {
        tmpQuery.append((*itt).offCommand);
    }

    sendDataToPort(tmpQuery);
    if(devConfig.startTECwithLaser) tecButtonSlot();
}

void MainWindow::tecButtonSlot() {
    if(!portIsOpen) return;
    bool state;

    QMap<QString, doubleMaskCommand_t>::iterator itt;
    itt = devConfig.stateButtons.find("tec");
    if(itt == devConfig.stateButtons.end()) return;

    QString tmpQuery = QString(COM_WRITE_PREFIX) + (*itt).code + QString(" ");

    if (devConfig.startTECwithLaser) {
        state = ui->laserButton->isChecked();
    } else {
        state = ui->tecButton->isChecked();
    }

    if(state)
        tmpQuery.append((*itt).onCommand);
    else
        tmpQuery.append((*itt).offCommand);

    sendDataToPort(tmpQuery);
}

void MainWindow::sendDataToPort(QString msg) {
    msg = msg.toUpper();
    if(!serialPort->isOpen()) return;

    serialPort->putIntoQueue(msg);
}

void MainWindow::toogleCompactModeSlot(bool state) {
    settings.setCompactModeFlag(state);

    settings.setHideControlsFlag(false);
    ui->controlsVisibleButton->setText(tr("Hide controls"));
    ui->controlsVisibleButton->setChecked(false);

    if(devConfig.paramWidgets.isEmpty()) return;

    for (QList<ParameterForm*>::iterator item = devConfig.paramWidgets.begin(); item < devConfig.paramWidgets.end(); item++) {
        if(state) {
            (*item)->loadCompactWidget()->setVisible(true);
            (*item)->loadWidget()->setVisible(false);
            ui->parametersGroupBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

        } else {
            (*item)->loadCompactWidget()->setVisible(false);
            (*item)->loadWidget()->setVisible(true);
            ui->parametersGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
        }
    }

    updateWindow();
}

void MainWindow::updateWindow() {
    ui->parametersGroupBox->adjustSize();
    ui->parametersGroupBox->layout()->update();
    ui->centralWidget->adjustSize();
    ui->actualParameters->layout()->update();
    this->adjustSize();
    this->setMaximumSize(this->size());
}


void MainWindow::spcialParameterSlot(QString paramLabel) {
    int i = 0;
    QString tmpQuery = QString(COM_WRITE_PREFIX);
    specParams_t obj;

    for(i = 0; i < devConfig.specialParameters.count(); i++)
        if(paramLabel.compare(devConfig.specialParameters.at(i).label, Qt::CaseInsensitive) == 0) break;

    obj = devConfig.specialParameters.at(i);
    tmpQuery += obj.code + QString(" ");

    if(obj.cbPtr->isChecked()) {
        tmpQuery += obj.onCommand;
    } else {
        tmpQuery += obj.offCommand;
    }
    sendDataToPort(tmpQuery);

}

void MainWindow::loadFont() {
    QFontDatabase::addApplicationFont(":/font/ShareTechMono.ttf");
    systemFont = QFont("ShareTechMono");
}

void MainWindow::saveWindowSettings() {
    settings.setWindowPosition(pos());
//    settings.setWindowSize(size());
}

void MainWindow::loadWindowSettings() {
    QPoint pos = settings.getWindowPosition();
//    QSize size = settings.getWindowSize();

    if(pos != WINDOW_DEFAULT_POSITION) { //&& size != AppSettings::WINDOW_DEFAULT_SIZE) {
        move(pos);
//        resize(size);
    }
}

bool MainWindow::maybeSave() {
    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, APP_NAME_TITLE,
                               tr("Do you really want to close it?"),
                               QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes)
        return true;
    else
        return false;
}