#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(AppSettings *appSettings, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings(appSettings)
{
    ui->setupUi(this);

    if(appSettings == nullptr) {
        qDebug() << "ComPort" << "appSettings pointer is NULL";
    }

    setupWindow();
    setConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if(showWarningMessageAndStopLaser()) {
//        checkStopAndDisconnect = true;
//        autoSendNextCommand = false;
//        serialPort->stopAndDisconnect();
        event->ignore();
        // do stopping of laser
    } else {
        event->accept();
    }
}

void MainWindow::setConnections() {
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
    connect(ui->actionKeep_checkboxes, &QAction::triggered, [this](bool flag){
        if(flag) {
            this->saveCheckboxes();
        }
        settings->setKeepCheckboxesFlag(flag);
    });

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
    // Если произошел таймаут СОМ-порта - сообщаем в главное окно
    QObject::connect(serialPort, SIGNAL(portTimeroutOccure()), this, SLOT(comPortTimeout()));
    // Произошла ошибка COM-порта - пишем в лог и консоль
    QObject::connect(serialPort, SIGNAL(errorOccuredSignal(QString)), this, SLOT(comPortError(QString)));

    // Если установленно "автоподключение", то устанавливаем связь с ком-портом при запуске программы
    if(settings->getComAutoconnectFlag()) serialPort->setPortState(true);

    // Обработка нажатия кнопки начала\остановки авторассылки команд
    connect(ui->consoleStartStopButton, SIGNAL(clicked(bool)), this, SLOT(comSetDataTransfer(bool)));
    // Кнопка посыла следующей команды при паузе авторассылки команд
    connect(ui->consoleNextCommandButton, SIGNAL(clicked(bool)), this, SLOT(sendNextComCommand()));

    // Подключение слотов-обработчиков нажатия кнопок LASER и TEC
    connect(ui->laserButton, SIGNAL(clicked(bool)), this, SLOT(laserButtonSlot()));
    connect(ui->tecButton, SIGNAL(clicked(bool)), this, SLOT(tecButtonSlot()));

    // Настройка и создание экземпляра окна "о программе"
    connect(aboutDialog, SIGNAL(startUpdate()), this, SLOT(startUpdateSlot()));
    connect(aboutDialog, SIGNAL(showConsoleSignal(bool)), this, SLOT(showConsoleSlot(bool)));

    // Обработчик нажатия кнопки device options
    connect(ui->devAboutButton, SIGNAL(clicked(bool)), aboutDeviceDialog, SLOT(show ()));
    // Обработчик нажатия кнопки Select device для устройств общего id
    connect(ui->devSelectButton, SIGNAL(clicked(bool)), this, SLOT(reselectDevice()));

    // Сигналы логгера
    // Отправка ошибок логгера в консоль основного окна
    QObject::connect(logger, SIGNAL(errorOccuredSignal(QString)), this, SLOT(writeToConsoleError(QString)));

    // Отправка модели выбранного драйвера (для изделий со старым айди) в главную программу
    connect(selectDeviceDialog, SIGNAL(selectedDevice(QString)), this, SLOT(selectedDeviceSlot(QString)));

    // XML praser
    // Если загрузка списка устройств завершилась успешно - вызываем соответствующую процедуру
    connect(xml, SIGNAL(endOfLoadingProgramConfig(bool)), this, SLOT(loadConfigProgramFinished(bool)));
    // Если загрузка параметров устройства завершилась успешно - вызываем соответствующую процедуру
    connect(xml, SIGNAL(endOfLoadingConfig(bool)), this, SLOT(loadConfigFinished(bool)));
    // Если загрузка произошла с ошибкой, пишем в консоль и в лог ошибку
    connect(xml, SIGNAL(errorHandler(QString)), this, SLOT(writeToConsoleError(QString)));
    connect(xml, &xmlReader::logger, logger, &consoleLogger::writeToLog);
}

void MainWindow::setupWindow() {
    devID = 0;
    loadFont();
    setWindowTitle(appTitle);
    setWindowIcon(QIcon(":/images/logo-minimal.png"));

    loadWindowSettings();
    settings->setLastSelectedDeviceId(0);

    // Временно скрываем меню help
    ui->actionHelp->setVisible(false);

    // Определение вспомогательных классов
    logger = new consoleLogger();
    serialPort = new ComPort(settings);
    changeLimitsDialog = new ChangeLimitsDialog(this);
    calibrateDialog = new CalibrateDialog(this);
    xml = new xmlReader(this);
    actualParamsGLayout = new QGridLayout();
    aboutDeviceDialog = new AboutDeviceDialog(settings, this);
    bitsLayout = new BitsLayout(ui->bitMaskBox);
    ui->bitMaskBox->setLayout(bitsLayout->getLayout());
    selectDeviceDialog = new SelectDeviceDialog(this);

    currCommandItt = nullptr;
    oldDevID = 0;
    autoSendNextCommand = true;
    requestAllCommands = true;
    bNeedSetCheckboxes = false;
    bStatusHasLoaded = false;
    bNeedSaveCheckboxes = false;
//    checkStopAndDisconnect = false;

    loadCommonConfig(availableDevices);

    ui->comPortConnectButton->setText(tr("Disconnected"));
    portIsOpen = false;
    isDeviceLoaded = false;

    ui->actionAutoconnect->setChecked(settings->getComAutoconnectFlag());
    ui->actionSave_settings->setEnabled(false);
    ui->actionLoad_settings->setEnabled(false);
    ui->actionKeep_checkboxes->setEnabled(false);
    ui->controlsVisibleButton->setChecked(false); //settings.getHideControlsFlag());
    ui->controlsVisibleButton->setText("Hide controls");

    setupMenuView();
    setupMenuPort();
    refreshMenuFile();
    ui->actionExit->setShortcut(QKeySequence("ALT+F4"));
    ui->actionKeep_checkboxes->setChecked(settings->getKeepCheckboxesFlag());

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

    if(debugMode == false) showConsoleSlot(false);

    devConfig.laserOn = false;
    devConfig.tecOn = false;
}

bool MainWindow::showWarningMessageAndStopLaser() {
    bool result = false;

    if(devConfig.laserOn || devConfig.tecOn) {
        QMessageBox *alertBox = new QMessageBox(this);
        alertBox->setIcon(QMessageBox::Warning);
        alertBox->setText("The device is still running!");
        alertBox->setInformativeText("Do you want to exit the app without stop the laser?");
        alertBox->addButton("OK", QMessageBox::AcceptRole);
        QPushButton *stopButton = alertBox->addButton("Cancel", QMessageBox::RejectRole);
        alertBox->setDefaultButton(stopButton);

        switch(alertBox->exec()) {
            case QMessageBox::AcceptRole:
                result = false;
                break;
        case QMessageBox::RejectRole:
            result = true;
            break;
        }
    }
    return result;
}

//void closeSoftware();
//void stopAndCloseSoftware();

void MainWindow::setupMenuPort() {
    // Формирование и вывод в меню списка доступных бауд-рейтов
    ui->menuSelectBaudrate->clear();
//    QSignalMapper* signalMapper = new QSignalMapper(this);
    foreach (quint32 BR, comBaudRates) {
        QAction *newAct = new QAction(QString::number(BR), this);
        newAct->setCheckable(true);
        if(settings->getComBaudrate() == BR) {
            newAct->setChecked(true);
        } else {
            newAct->setChecked(false);
        }
        QFont actionFont = newAct->font();
        actionFont.setFamily("Share Tech Mono");
        newAct->setFont(actionFont);
        ui->menuSelectBaudrate->addAction(newAct);
        connect(newAct, &QAction::triggered, [this, BR]{
            this->changeBaudRateSlot(BR);
        });
//        signalMapper->setMapping(newAct, BR);
//        connect(newAct, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
    }
//    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(changeBaudRateSlot(int)));

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
//        QSignalMapper* signalMapper = new QSignalMapper(this);
        foreach (QString port, portList) {
            QAction *newAct = new QAction(port, this);
            newAct->setCheckable(true);
            if(QString::compare(settings->getComPort(), port, Qt::CaseInsensitive) == 0) {
                newAct->setChecked(true);
            } else {
                newAct->setChecked(false);
            }
            ui->menuSelectPort->addAction(newAct);
            connect(newAct, &QAction::triggered, [this, port]{
                settings->setComPort(port);
                refreshMenuPort();
            });
//            signalMapper->setMapping(newAct, str);
//            connect(newAct, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
        }
//        connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(changePortSlot(QString)));
    }
    // кнопка обновления портов
    QAction *refreshPortAct = new QAction(tr("Refresh list"), this);
    ui->menuSelectPort->addSeparator();
    ui->menuSelectPort->addAction(refreshPortAct);
    connect(refreshPortAct, SIGNAL(triggered(bool)), this, SLOT(refreshMenuPort()));

    foreach (QAction *act, ui->menuSelectPort->actions()) {
        if(QString::compare(settings->getComPort(), act->text(), Qt::CaseInsensitive) == 0) {
            act->setChecked(true);
        } else {
            act->setChecked(false);
        }
    }
}

void MainWindow::refreshMenuBaud() {
    foreach (QAction *act, ui->menuSelectBaudrate->actions()) {
        if(settings->getComBaudrate() == act->text().toUInt()) {
            act->setChecked(true);
        } else {
            act->setChecked(false);
        }
    }
}

void MainWindow::refreshMenuStopBits() {
    foreach (QAction *act, ui->menuStop_bits->actions()) {
        if(act->text() == "One stop bit") {
            if(settings->getComStopBits() == 1) {
                act->setChecked(true);
            } else {
                act->setChecked(false);
            }
        } else if(act->text() == "Two stop bits") {
            if(settings->getComStopBits() == 2) {
                act->setChecked(true);
            } else {
                act->setChecked(false);
            }
        }
    }
}

void MainWindow::refreshMenuCalibrate() {
    if(devConfig.calCoefs.isEmpty()) {
        ui->menuCalibrate->menuAction()->setVisible(false);
    } else {
        ui->menuCalibrate->menuAction()->setVisible(true);
    }

    if(!menuCalibrateActions.isEmpty()) {
        ui->menuCalibrate->clear();
        menuCalibrateActions.clear();
    }

    ui->menuCalibrate->setEnabled(true);

    foreach(calibration_t item, devConfig.calCoefs) {
        QAction *newAction = new QAction();
        newAction->setText(item.title);
        ui->menuCalibrate->addAction(newAction);
        menuCalibrateActions.append(newAction);
        connect(newAction, &QAction::triggered, [this, item]{
            this->openCalibrateWindow(item.title);
        });
    }
}


void MainWindow::openCalibrateWindow(QString name) {
    foreach(const calibration_t item, devConfig.calCoefs) {
        if(item.title == name) {
            calibrateDialog->setStruct(item);
            calibrateDialog->show();
            break;
        }
    }
}

void MainWindow::refreshMenuLimits() {
    if(devConfig.limits.isEmpty()) {
        ui->menuLimits->menuAction()->setVisible(false);
    } else {
        ui->menuLimits->menuAction()->setVisible(true);
    }

    if(!menuLimitsActions.isEmpty()) {
        ui->menuLimits->clear();
        foreach(QAction* item, menuLimitsActions) {
            item->deleteLater();
        }
        menuLimitsActions.clear();
    }

    ui->menuLimits->setEnabled(true);

    foreach(DeviceLimit* limit, devConfig.limits) {
        QAction *newAction = new QAction();
        newAction->setText(limit->getTitle());
        ui->menuLimits->addAction(newAction);
        menuLimitsActions.append(newAction);
        connect(newAction, &QAction::triggered,[this, limit]{
            this->openLimitsWindow(limit->getTitle());
        });
    }
}

void MainWindow::openLimitsWindow(QString name) {
    foreach(DeviceLimit* limit, devConfig.limits) {
        if(limit->getTitle() == name) {
            changeLimitsDialog->setData(limit);
//            foreach(Command* param, devConfig.commands) {
//                if(param->getCode().compare(limit->getUpperLimitCode()) == 0) {
//                    changeLimitsDialog->setAbsMax(param->getConvertedValue());
//                } else if(param->getCode().compare(limit->getBottomLimitCode()) == 0) {
//                    changeLimitsDialog->setAbsMin(param->getConvertedValue());
//                } else if(param->getCode().compare(limit->getMaxCode()) == 0) {
//                    changeLimitsDialog->setMax(param->getConvertedValue());
//                } else if(param->getCode().compare(limit->getMinCode()) == 0) {
//                    changeLimitsDialog->setMin(param->getConvertedValue());
//                }
//            }

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

//    QSignalMapper* signalMapper = new QSignalMapper(this);
    ui->menuFile->addSeparator();
    foreach(QVariant listVar, settings->getRecentOpenFiles()) {
        QString fileName = listVar.toString();
        // Проверка существует ли файл.
        if(!QFile::exists(fileName)) {
            settings->removeRecentOpenFiles(fileName);
            continue;
        }

        QAction *newAct = new QAction(fileName, ui->menuFile);
        connect(newAct, &QAction::triggered, [this, fileName]{
            this->readSettingsFile(fileName);
        });
//        signalMapper->setMapping(newAct, fileName);
//        connect(newAct, SIGNAL(triggered(bool)), signalMapper, SLOT(map()));
        newAct->setEnabled(link);
        lastFileActions.prepend(newAct);
        ui->menuFile->addAction(newAct);
    }
//    connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(readSettingsFile(QString)));
}



void MainWindow::readSettingsFile(QString fileName) {
    if(!QFile::exists(fileName)) {
        settings->removeRecentOpenFiles(fileName);
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
                sendDataToPort(msgForSend);
            }
        }
    }

    autoSendNextCommand = autoSend;

    delete file;
}

void MainWindow::loadSettingsSlot() {
    QString lastDir = settings->getLastSaveDirectory();
    QDir dir = QDir(lastDir);
    if(!dir.exists()) lastDir = QDir::homePath();

    QFileDialog *fd = new QFileDialog();
    fd->setDirectory(lastDir);
    fd->setFileMode(QFileDialog::ExistingFile);
    QString filename = fd->getOpenFileName(this, tr("Select file"), settings->getLastSaveDirectory(), tr("Settings files (*.msf)"));
    if(!filename.isEmpty()) {
        settings->setLastSaveDirectory(fd->directory().absolutePath());
        readSettingsFile(filename);
    }
}


void MainWindow::saveSettingsSlot() {
    if(!isDeviceLoaded) return;

    QStringList dataOut;

    QString lastDir = settings->getLastSaveDirectory();
    QDir dir = QDir(lastDir);
    if(!dir.exists()) lastDir = QDir::homePath();

    QFileDialog *fd = new QFileDialog();
    fd->setFileMode(QFileDialog::AnyFile);
    fd->setDirectory(lastDir);
    QString filename = fd->getSaveFileName(this, tr("Select file"), settings->getLastSaveDirectory(), tr("Settings files (*.msf)"));
    if(!filename.isEmpty()) {
        settings->setLastSaveDirectory(fd->directory().absolutePath());
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

        bool freqIsForward = false;
        bool durationIsForward = false;


        foreach(Command* commPtr, devConfig.commands) {
            if(commPtr->getCode() == DEVICE_STATUS_COMMAND) continue;
            if(commPtr->getCode() == DURATION_COMMAND && !freqIsForward) {
                durationIsForward = true;
                continue;
            }


            currentLine.clear();
            currentLine = QString("%1:%2").arg((*commPtr).getCode()).arg((*commPtr).getRawValue(), 4, 16, QChar('0'));
            dataOut << currentLine.toUpper() << endl;

            if(commPtr->getCode() == FREQUENCY_COMMAND) {
                freqIsForward = true;
                if(durationIsForward) {
                    currentLine.clear();
                    currentLine = QString("%1:%2").arg(DURATION_COMMAND).arg(devConfig.commands.value(DURATION_COMMAND)->getRawValue(), 4, 16, QChar('0'));
                    dataOut << currentLine.toUpper() << endl;
                }
            }
        }

        if(devConfig.commands.contains(DEVICE_STATUS_COMMAND)) {

            foreach(binOption_t cb, devConfig.binOptions) {
                currentLine.clear();
                QString value = "";
                if(cb.checkBox->isChecked()) {
                    value = cb.onCommand;
                } else {
                    value = cb.offCommand;
                }
                currentLine = QString("%1:%2").arg(DEVICE_STATUS_COMMAND).arg(value);
                dataOut << currentLine.toUpper() << endl;
            }

        }


        file->close();

        autoSendNextCommand = tmpAutoSend;
        // Процесс записи окончен
        QList<QVariant> tmpList = settings->getRecentOpenFiles();
        if(tmpList.contains(filename)) {
            tmpList.removeAll(filename);
            settings->removeRecentOpenFiles(filename);
        }
        if(tmpList.size() >= MAX_FILES_IN_MENU) tmpList.removeLast();
        tmpList.prepend(filename);
        settings->setRecentOpenFiles(tmpList);

        refreshMenuFile();

        file->deleteLater();
    }
}

void MainWindow::setupMenuView() {
    connect(ui->actionTemperature_in_C, &QAction::triggered, [this]{
        this->triggTemperatureSymbolSlot("C");
    });
    connect(ui->actionTemperature_in_F, &QAction::triggered, [this]{
        this->triggTemperatureSymbolSlot("F");
    });

    refreshMenuView();
}

void MainWindow::refreshMenuView() {
    if(QString::compare(settings->getTemperatureSymbol(), "C", Qt::CaseInsensitive) == 0) {
        ui->actionTemperature_in_C->setChecked(true);
        ui->actionTemperature_in_F->setChecked(false);
    } else {
        ui->actionTemperature_in_C->setChecked(false);
        ui->actionTemperature_in_F->setChecked(true);
    }
    ui->actionCompact_view->setChecked(settings->getCompactModeFlag());
}

void MainWindow::setupParameterHandlers() {

    serialPort->setStopCommandDelay(devConfig.stopCommandDelay);
    currCommandItt = nullptr;

    if(!devConfig.paramWidgets.empty()) {
        QGridLayout* layoutparams = new QGridLayout();

        ui->controlsVisibleButton->setChecked(false); //settings.getHideControlsFlag());
        ui->controlsVisibleButton->setText("Hide controls");
        aboutDeviceDialog->setInfo(devConfig);

        refreshMenuLimits();
        refreshMenuCalibrate();

        bitsLayout->addFromList(devConfig.leds);

        int rowcol = 0;
        int row = 0;
        foreach(ParameterController* parameterController, devConfig.paramWidgets) {
            parameterController->setEnableState(true);
            connect(parameterController, SIGNAL(changeValue(QString)), this, SLOT(sendDataToPort(QString)));

            actualParamsGLayout->addWidget(parameterController->loadTextWidget(), row, 0);
            row++;

            if(!parameterController->isOnlyMeasured()) { // Вывод "крутилок-таскалок" для параметров
                // Сокрытие\показ дополнительных полей если параметр доступен только для записи или для чтения\записи
                parameterController->hideRealValue(!parameterController->hasRealCommand());

                if(settings->getCompactModeFlag()) {
                    parameterController->loadCompactWidget()->setVisible(true);
                    parameterController->loadWidget()->setVisible(false);
                } else {
                    parameterController->loadCompactWidget()->setVisible(false);
                    parameterController->loadWidget()->setVisible(true);
                }

                layoutparams->addWidget(parameterController->loadWidget(), 0, rowcol);
                layoutparams->addWidget(parameterController->loadCompactWidget(), rowcol+1, 0);
                parameterController->loadTextWidget()->setVisible(false);
                rowcol++;
            } else { // Вывод значений в окно параметров
                parameterController->loadTextWidget()->setVisible(true);
            }
        }

        // Крутилки-вертелки для параметром
        if(ui->parametersGroupBox->layout()) delete ui->parametersGroupBox->layout();
        ui->parametersGroupBox->setLayout(layoutparams);
        // Вывод значений в окне параметров
        ui->actualParameters->setLayout(actualParamsGLayout);

        // Формирование и вывод на экран панели режимов
        if(!devConfig.binOptions.empty()) {
            QVBoxLayout *vlayout = new QVBoxLayout();
            foreach(binOption_t binOption, devConfig.binOptions) {
                binOption.checkBox->setStyleSheet("QCheckBox {font-family: \"Share Tech Mono\"; border: none; color: #fff;} ");
                vlayout->addWidget(binOption.checkBox);

                connect(binOption.checkBox, &QCheckBox::clicked, [=]{
                    QString commandStr = binOption.offCommand;
                    if(binOption.checkBox->isChecked()) {
                        commandStr = binOption.onCommand;
                    }
                    QString sQuery = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(binOption.code).arg(commandStr);
                    this->sendDataToPort(sQuery);
                    bNeedSaveCheckboxes = true;
                });
            }
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

    if(isDeviceLoaded && portIsOpen) prepareToSendNextCommand();

    updateWindow();
}

bool MainWindow::isCheckboxesFileExist() {
    bool state = false;
    QDir *dir = new QDir(saveParDir);
    if(!dir->exists()) {
        state = false;
        writeToConsole(QString("Dir (%1) is not exists").arg(saveParDir));
    }
    QString filename = QString("%1/%2%3%4").arg(saveParDir).arg(saveParFilenamePrefix).arg(QString::number(devID, 16)).arg(".cfg");
    QFile *file = new QFile(filename);
    if(file->exists()) {
        state = true;
    } else {
        writeToConsole(QString("File (%1) is not exists").arg(filename));
    }
    file->deleteLater();
    return state;
}

void MainWindow::checkStateAndDoIt(Command* cmd) {
    if(loadedValues.contains(cmd->getCode())) {
        quint16 targetState = loadedValues.value(cmd->getCode());

        for(binOption_t item : devConfig.binOptions) {
            if(item.code == cmd->getCode()) {
                    quint16 currentState = static_cast<quint16>(cmd->getRawValue());
                    quint16 maskState = currentState ^ targetState;
                    if (maskState & item.mask) {
                        QPair<QString, QString> pair;
                        if(targetState & item.mask) {
                            pair = qMakePair(item.code, item.onCommand);
                        } else {
                            pair = qMakePair(item.code, item.offCommand);
                        }
                        sendDataToPort(COM_WRITE_PREFIX + pair.first + " " + pair.second);
                    }
            }
        }
        loadedValues.remove(cmd->getCode());
    }
}

void MainWindow::getDefaultCheckboxesValues() {
    loadedValues.clear();
    if(devConfig.binOptions.isEmpty()) return;

    for(binOption_t item : devConfig.binOptions) {
        if(loadedValues.contains(item.code)) {
            loadedValues[item.code] |= item.mask;
        } else {
            loadedValues.insert(item.code, item.mask);
        }
    }
}

void MainWindow::getNewCheckboxesValues() {
    loadedValues.clear();
    if(devConfig.binOptions.isEmpty()) return;


    QFile *file = new QFile(QString("%1/%2%3%4").arg(saveParDir).arg(saveParFilenamePrefix).arg(QString::number(devID, 16)).arg(".cfg"));
        if(file->open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(file);
            while(!in.atEnd()) {
                QStringList rLine = in.readLine().split(":", QString::SkipEmptyParts, Qt::CaseInsensitive);
                if(rLine.size() >= 2) {
                    QString code = rLine.at(0);
                    quint16 state = static_cast<quint16>(rLine.at(1).toUInt());
                    loadedValues.insert(code, state);
                }
            }
            file->close();
        }
    if(file->error()) {
        writeToConsoleError(file->errorString());
    }

    file->deleteLater();
}

void MainWindow::saveCheckboxes() {
    if(devConfig.binOptions.isEmpty()) return;

    QDir *dir = new QDir(saveParDir);
    if(!dir->exists()) {
        QDir().mkdir(saveParDir);
    }

    QFile *file = new QFile(QString("%1/%2%3%4").arg(saveParDir).arg(saveParFilenamePrefix).arg(QString::number(devID, 16)).arg(".cfg"));
    file->open(QIODevice::WriteOnly | QIODevice::Text);
    if(file->error()) {
        writeToConsoleError(file->errorString());
    }

    QTextStream dataOut(file);
    QString currentLine;
    QList<QString> codes;

    for(binOption_t binOption : devConfig.binOptions) {
        currentLine.clear();
        QString value = "";
        if(!codes.contains(binOption.code)) {
            Command *cmd = devConfig.commands.value(binOption.code, nullptr);
            if(cmd != nullptr) {
                currentLine = QString("%1:%2").arg(binOption.code).arg(cmd->getRawValue());
                dataOut << currentLine.toUpper() << endl;
            }
            codes.append(binOption.code);
        }
    }
    codes.clear();


    file->close();

    file->deleteLater();
}

void MainWindow::clearAllRegulators() {
    foreach(ParameterController* item, devConfig.paramWidgets) {
        item->deleteLater();
    }

    devConfig.paramWidgets.clear();
    loadedValues.clear();

    bitsLayout->clear();

    foreach(Command* cmd, devConfig.commands) {
        delete cmd;
    }


    foreach(binOption_t item, devConfig.binOptions) {
         delete item.checkBox;
    }


    devConfig.limits.clear();
    devConfig.calCoefs.clear();
    devConfig.commands.clear();
    devConfig.description = "";
    devConfig.devName = "";
    devConfig.hasLaser = false;
    devConfig.hasTEC = false;
    devConfig.image = "";
    devConfig.link = "";
    devConfig.paramWidgets.clear();
    devConfig.binOptions.clear();
    devConfig.stateButtons.clear();
    devConfig.leds.clear();

    if(ui->parametersGroupBox->layout()) delete ui->parametersGroupBox->layout();
    bitsLayout->clear();
    if(ui->specialParamBox->layout()) delete ui->specialParamBox->layout();

    this->updateWindow();
}

void MainWindow::setRegulatorsEnable(bool state) {
    for(qint8 i = 0; i < devConfig.paramWidgets.count(); i++) {
        ParameterController *item = devConfig.paramWidgets.at(i);
        item->setEnableState(state);
    }

    for(qint8 i = 0; i < devConfig.binOptions.count(); i++) {
        devConfig.binOptions.at(i).checkBox->setEnabled(state);
    }

    comSetDataTransfer(state);
    if(state) {
        requestAllCommands = true;
    }

    if(devConfig.hasTEC) {
        ui->tecButton->setVisible(state);
    }

    if(devConfig.hasLaser) {
        ui->laserButton->setVisible(state);
    }

    foreach(QAction* item, ui->menuLimits->actions()) {
        item->setEnabled(state);
    }

    foreach(QAction* item, ui->menuCalibrate->actions()) {
        item->setEnabled(state);
    }
}

void MainWindow::getPortNewState(bool state) {
    if (state != portIsOpen) {
        if(state) { // Port is openned now
            this->writeToConsole(settings->getComPort() + tr(" is open!\n"), CONSOLE_INFO_COLOR);
            ui->comPortConnectButton->setText(settings->getComPort() + QString("   ") + QString::number(settings->getComBaudrate()) + "bps");
            startDeviceIdent();
            if(ui->statusBar->currentMessage().size() > 0) ui->statusBar->showMessage("");
        } else { // Port is closed now
            this->writeToConsole(settings->getComPort() + tr(" is close!\n"), CONSOLE_INFO_COLOR);
            ui->comPortConnectButton->setText(tr("Disconnected"));
            devID = 0;
            settings->setLastSelectedDeviceId(0);
            setRegulatorsEnable(false);
            setLink(false);
            isDeviceLoaded = false;
            devConfig.laserOn = false;
            devConfig.tecOn = false;
            bNeedSaveCheckboxes = false;
        }
        ui->consoleStartStopButton->setEnabled(state);
        ui->consoleStartStopButton->setChecked(!autoSendNextCommand);
    }

    portIsOpen = state;
    ui->comPortConnectButton->setChecked(!portIsOpen);

    ui->actionKeep_checkboxes->setEnabled(portIsOpen);
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

    QString tmpStr = QString("%1%2").arg(COM_REQUEST_PREFIX).arg(IDENTIFY_DEVICE_COMMAND, 4, 16, QChar('0'));
    sendDataToPort(tmpStr);
}

void MainWindow::comPortConnectButton_slot()
{
    if(portIsOpen) {
        if(showWarningMessageAndStopLaser()) {
//            serialPort->stopAndDisconnect();
        } else {
            serialPort->setPortState(!portIsOpen);
        }
    } else {
        serialPort->setPortState(!portIsOpen);
    }
    ui->comPortConnectButton->setChecked(!portIsOpen);
}

void MainWindow::comPortConsoleSend_Slot() {
    QString str = ui->comPortConsoleTextEdit->text();
    ui->comPortConsoleTextEdit->clear();
    sendDataToPort(str);
}

void MainWindow::readComData_Slot(QByteArray str) {
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
                    if(devID == 0) {
    //                    devID = settings.getLastSelectedDeviceId();
                        bool isCommonDevice = false;
                        foreach(availableDev_t devStruct, availableDevices) {
                            if(devStruct.id == devID) {
                                isCommonDevice = true;
                                break;
                            }
                        }

                        if(!isCommonDevice) {
                            writeToConsole("Please, select deivce model.");
                            reselectDevice();
                        }
                    } else {
                        setRegulatorsEnable(true);
                        ui->devNameLabel->setText(devConfig.devName);
                        ui->devSelectButton->setVisible(true);
                        ui->devAboutButton->setVisible(true);
                        oldDevID = devID;
                    }
                } else {
                    devID = value;
                    if(devID == oldDevID && isDeviceLoaded) {
                        setRegulatorsEnable(true);
                        ui->devNameLabel->setText(devConfig.devName);
                        ui->devSelectButton->setVisible(true);
                        ui->devAboutButton->setVisible(true);
                        oldDevID = devID;
                    } else {
                        clearAllRegulators();
                        loadDeviceConfig(devID);
                    }
                }

            } else {
                if (devConfig.commands.contains(commandStr)) {
                    Command* currentCommand = devConfig.commands.value(commandStr);
                    currentCommand->setRawValue(value);


                    if(ui->actionKeep_checkboxes->isChecked()) {
                        if(bNeedSetCheckboxes) {
                            checkStateAndDoIt(currentCommand);
                            if(loadedValues.isEmpty()) bNeedSetCheckboxes = false;
                        }
                    }

                    // Обработка LEDS
                    setLedState(commandStr, value);

                    // Обработка галочек
                    for(binOption_t binaryOption : devConfig.binOptions) {
                        if(binaryOption.mask == 0) continue;

                        if(commandStr == binaryOption.code) {
                            if(value & binaryOption.mask) {
                                binaryOption.checkBox->setChecked(true);
                            } else {
                                binaryOption.checkBox->setChecked(false);
                            }
                        }
                    }

                    if(commandStr == DEVICE_STATUS_COMMAND) {
                        if(bNeedSaveCheckboxes) {
                            bNeedSaveCheckboxes = false;
                            saveCheckboxes();
                        }

                        if(devConfig.hasLaser) {
                            if(currentCommand->getRawValue() & START_STOP_MASK) {
                                ui->laserButton->setChecked(true);
                                devConfig.laserOn = true;
                            } else {
                                ui->laserButton->setChecked(false);
                                devConfig.laserOn = false;
                            }
                        }
                    } else
                        if(commandStr == TEC_STATUS_COMMAND) {
                            if(devConfig.hasTEC) {
                                if(currentCommand->getRawValue() & START_STOP_MASK) {
                                    ui->tecButton->setChecked(true);
                                    devConfig.tecOn = true;
                                } else {
                                    ui->tecButton->setChecked(false);
                                    devConfig.tecOn = false;
                                }
                            }
                        }
                }
            }
        } else if (prefix == COM_ERROR_PREFIX) {
            this->writeToConsoleError("<- "+str);
            switch(command) {
            case ERROR_BUFFER_OVERLOAD:
            case ERROR_WRONG_COMMAND:
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
            clearAllRegulators();
            devID = devStruct.id;
            settings->setLastSelectedDeviceId(devID);
            loadDeviceConfig(devID);
            serialPort->clearQueue();
            ui->devSelectButton->setVisible(true);
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
        devID = devConfig.deviceID;

        setupParameterHandlers();
        writeToConsole(tr("CONFIG: Config is loaded successful!"));

        foreach(Command* cmd, devConfig.commands) {
            cmd->resetInterval();
        }

        setRegulatorsEnable(true);
        bNeedSetCheckboxes = true;
        // Проверяем, есть ли сохраненные конфигурации
        if(isCheckboxesFileExist()) {
            getNewCheckboxesValues();
        } else {
            // Конфигураций нет
            // Тогда ставим по-умолчанию все в состояние ВКЛ
            getDefaultCheckboxesValues();
        }
//        prepareToSendNextCommand();
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

void MainWindow::loadDeviceConfig(quint16 id) {
    xml->parseDeviceConfig(CONFIG_FILE, devConfig, id);
}

void MainWindow::loadCommonConfig(QList<availableDev_t> &deviceList) {
    xml->parseCommonConfig(CONFIG_FILE, deviceList, comBaudRates);
}

void MainWindow::comPortTimeout() {
    oldDevID = devID;
    devID = 0;
    setLink(false);
    setRegulatorsEnable(false);
    startDeviceIdent();

    emit writeToConsoleError(tr("The device didn't response. Com-port timeout: ") + QString::number(COM_PORT_TIMEOUT) + tr("ms"));
}

void MainWindow::comPortError(QString str) {
    oldDevID = devID;
    devID = 0;
    setLink(false);
    setRegulatorsEnable(false);
    QTimer sendTimer;
    sendTimer.singleShot(settings->getComCommandsDelay(), this, SLOT(startDeviceIdent()));
    writeToConsoleError("COM [ERROR]: " + str);
    ui->statusBar->showMessage("[ERROR]: " + str, STATUSBAR_MESSAGE_TIMEOUT);
}

void MainWindow::changeBaudRateSlot(int BR) {
    serialPort->changeBaudRate(BR);
    refreshMenuBaud();
}

void MainWindow::triggComAutoConnectSlot(bool state) {
    settings->setComAutoconnectFlag(state);
}

void MainWindow::triggTemperatureSymbolSlot(QString str) {
    settings->setTemperatureSymbol(str);

    foreach(Command* cmd, devConfig.commands) {
        if(cmd->isTemperature()) {
            cmd->setTemperatureUnit(str);
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
    if(!filedownloader.isNull()) {
        connect(filedownloader, nullptr, nullptr, nullptr);
        filedownloader.clear();
    }

    filedownloader = new FileDownloader(QUrl(UPDATE_LIST_URL) , this);
    connect(filedownloader, SIGNAL(downloaded()), this, SLOT(updateRepDownloadedSlot()));
}

void MainWindow::updateRepDownloadedSlot() {
    writeToConsole("Checking for updates...");

    QString data = QString(filedownloader->downloadedData());
    QStringList strList = data.split(";");
    QStringList versionList = strList.at(0).split(".");
    QString url = strList.at(1);

    if (url.isEmpty() || versionList.isEmpty()) return;

    uint32_t newVersion = 0;
    if(versionList.length() == 3) {
        newVersion = 255*255*versionList.at(0).toUInt() + 255*versionList.at(1).toUInt() + versionList.at(2).toUInt();
    } else {
        newVersion = 255*255*versionList.at(0).toUInt() + 255*versionList.at(1).toUInt();
    }
    uint32_t currenVersion = MAJOR_VERSION*255*255+MINOR_VERSION*255+PATCH_VERSION;

    updateUrl = url;

    if(newVersion > currenVersion) {
        QMessageBox *msgBox = new QMessageBox();
        msgBox->setStyleSheet("QMessageBox QLabel { color: rgb(255,255,255); } QMessageBox { background-color: rgb(51,51,51); }");
        QMessageBox::StandardButton answer;
        answer = msgBox->question(this, tr("Update is available"), tr("Software update is available. Do you want to update now?"), QMessageBox::Yes|QMessageBox::No);
        if(answer == QMessageBox::Yes) {
            startUpdateSlot();
            writeToConsole("Update is available. Version " + strList.at(0) + ". Update is starting!");
        }
        msgBox->deleteLater();
    } else {
        // Оповещение об отсутствии обновлений.
        ui->statusBar->showMessage("There is no updates available.", STATUSBAR_MESSAGE_TIMEOUT);
        writeToConsole("There is no updates available.");
    }
}


void MainWindow::startUpdateSlot() {
    QDesktopServices::openUrl(QUrl(updateUrl));
}

void MainWindow::sendLogSlot() {
    QDesktopServices::openUrl(QUrl::fromLocalFile(DEFAULT_LOG_FILEPATH));
}

void MainWindow::hideControlsButtonSlot(bool state) {
    settings->setHideControlsFlag(state);
    if(state) {
        ui->controlsVisibleButton->setText(tr("Show controls"));
    } else {
        ui->controlsVisibleButton->setText(tr("Hide controls"));
    }

    for(QList<ParameterController*>::iterator p = devConfig.paramWidgets.begin(); p != devConfig.paramWidgets.end(); p++) {
        if((*p)->getPinState() == false && !(*p)->isOnlyMeasured()) {
            if(settings->getCompactModeFlag()) {
                (*p)->loadCompactWidget()->setVisible(!state);
            } else {
                (*p)->loadWidget()->setVisible(!state);
            }

            if (state) { // скрыть крутилки
                if ((*p)->hasRealCommand()) {
                    (*p)->loadTextWidget()->setVisible(true);
                }
            } else { // показать крутилки
                if ((*p)->hasRealCommand()) {
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
//        ui->devSelectButton->setVisible(false);
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

        if(currCommandItt == devConfig.commands.constEnd()) {
            currCommandItt = devConfig.commands.constBegin();
//            comPortIntervalCounter++;
//            if(comPortIntervalCounter > MAX_COM_INTERVAL_COUNTER) comPortIntervalCounter = 1;
            requestAllCommands = false;
        }

//        if((*currCommandItt)->getInterval() == 0) {
//            needToSend = false;
//        } else {
//            needToSend = ((comPortIntervalCounter % (*currCommandItt)->getInterval()) == 0);
//        }

        needToSend = (*currCommandItt)->needToRequest();
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
    ui->hideConsoleButton->setChecked(state);
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
    settings->setCompactModeFlag(state);

    settings->setHideControlsFlag(false);
    ui->controlsVisibleButton->setText(tr("Hide controls"));
    ui->controlsVisibleButton->setChecked(false);

    if(devConfig.paramWidgets.isEmpty()) return;

    for (QList<ParameterController*>::iterator item = devConfig.paramWidgets.begin(); item < devConfig.paramWidgets.end(); item++) {
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
    ui->centralWidget->adjustSize();
    this->adjustSize();
}


void MainWindow::spcialParameterSlot(QString paramLabel) {
    int i = 0;
    QString tmpQuery = QString(COM_WRITE_PREFIX);
    binOption_t obj;

    for(i = 0; i < devConfig.binOptions.count(); i++)
        if(paramLabel.compare(devConfig.binOptions.at(i).label, Qt::CaseInsensitive) == 0) break;

    obj = devConfig.binOptions.at(i);
    tmpQuery += obj.code + QString(" ");

    if(obj.checkBox->isChecked()) {
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
    settings->setWindowPosition(pos());
//    settings.setWindowSize(size());
}

void MainWindow::loadWindowSettings() {
    QPoint pos = settings->getWindowPosition();
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
