
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "globals.h"
#include "commondefines.h"
#include <QMainWindow>
#include <QString>
#include <QSettings>
#include <QThread>
#include <QWidget>
#include <QCheckBox>
#include <QSignalMapper>
#include <QList>
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QDesktopServices>
#include <QMessageBox>
//#include <QInputDialog>
#include <QListIterator>
#include <QBoxLayout>
#include <QFontDatabase>

#include <QDebug>

#include "comport.h"
#include "consolelogger.h"
#include "xmlreader.h"
#include "parametercontroller.h"
#include "aboutdialog.h"
#include "filedownloader.h"
#include "bitslayout.h"
#include "changelimitsdialog.h"
#include "aboutdevicedialog.h"
#include "calibratedialog.h"
#include "selectdevicedialog.h"

extern AppSettings settings;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void closeEvent(QCloseEvent *event);
//    void resizeEvent(QResizeEvent *event);
    typedef enum {CLEAR_ALL_DATA, CLEAR_ONLY_WIDGETS} clearParamsOptions_t;
    typedef enum {NONE_CLOSE, CONDITION_CLOSE, APPROVE_CLOSE} closeWindow_t;
    const QString saveParDir = "cfgs/";
    const QString saveParFilenamePrefix = "checkboxes_";

private slots:
    void comPortConnectButton_slot();
    void comPortConsoleSend_Slot();
    void readComData_Slot(QByteArray str);
    void writeToConsole(QString str, Qt::GlobalColor color = CONSOLE_INFO_COLOR);
    void writeToConsoleError(QString str);
    void getPortNewState(bool);
    void startDeviceIdent();
    void loadConfigFinished(bool);
    void loadConfigProgramFinished(bool);
//    void writeToLogSlot(QString str);
    void comPortTimeout();
    void comPortError(QString);
    void changeBaudRateSlot(int BR);
    void changePortSlot(QString port);
    void triggComAutoConnectSlot(bool state);
    void triggTemperatureSymbolSlot(QString);
    void refreshMenuView();
    void refreshMenuPort();
    void refreshMenuBaud();
    void refreshMenuStopBits();
    void refreshMenuLimits();
    void refreshMenuCalibrate();
    void refreshMenuFile();
    void readSettingsFile(QString file);
    void loadSettingsSlot();
    void saveSettingsSlot();
    void showAboutDialogSlot();
    void openHelpSlot();
    void checkForUpdatesSlot();
    void updateRepDownloadedSlot();
    void startUpdateSlot();
    void sendLogSlot();
    void hideControlsButtonSlot(bool);
    void loadDeviceConfig(quint16 id = 0);
    void loadCommonConfig(QList<availableDev_t>& deviceList);
    void spcialParameterSlot(QString);
//    void paramPrepareToSendCommand(QString);
    //void setLastSentCommand(QString);
    void comSetDataTransfer(bool state);
    void showConsoleSlot(bool state);
    void laserButtonSlot();
    void tecButtonSlot();
    void sendDataToPort(QString msg);
    void sendNextComCommand();
    void prepareToSendNextCommand();
    void toogleCompactModeSlot(bool);
    void reselectDevice();
    void openLimitsWindow(QString name);
    void openCalibrateWindow(QString name);
    void setLedState(QString, quint16);
    void selectedDeviceSlot(QString);
    void setComOneStopBit();
    void setComTwoStopBits();
    void saveCheckboxes();
    void loadCheckboxes();

private:
    Ui::MainWindow *ui;
    bool showWarningMessageAndStopLaser();
    void setupWindow();
    void setConnections();
    void setupParameterHandlers();
    void clearAllRegulators();
    void setRegulatorsEnable(bool state);
    void setupMenuPort();
    void setupMenuView();
    void setLink(bool);
    void saveWindowSettings();
    void loadWindowSettings();
    bool maybeSave();
    bool isCheckboxesFileExist();
//    void setupIndicators();
//    bool selectDevice();
    void updateWindow();
    void loadFont();
    QStringList consoleBuffer;
    bool portIsOpen;
    QPointer<xmlReader> xml;
    device_t devConfig;
    QList<QAction*> lastFileActions;
    QList<QAction*> menuLimitsActions;
    QList<QAction*> menuCalibrateActions;
    QPointer<FileDownloader> filedownloader;
    QMap<QString, Command*>::const_iterator currCommandItt;
    quint16 oldDevID;
    bool link;
    QPointer<comPort> serialPort;
    QPointer<BitsLayout> bitsLayout;
    QPointer<QGridLayout> actualParamsGLayout;
    QList<QLabel*> actualParamsLabels;
    bool isDeviceLoaded;
    bool autoSendNextCommand;
    QPointer<AboutDialog> aboutDialog;
    QPointer<consoleLogger> logger;
    QList<availableDev_t> availableDevices;
    QPointer<ChangeLimitsDialog> changeLimitsDialog;
    QPointer<CalibrateDialog> calibrateDialog;
    QPointer<AboutDeviceDialog> aboutDeviceDialog;
    quint8 comPortIntervalCounter;
    QList<uint> comBaudRates;
    bool requestAllCommands;
    QFont systemFont;
    SelectDeviceDialog *selectDeviceDialog;
    quint16 devID;
//    closeWindow_t waitingForStop;
    QPushButton* saveBtn;
    QPushButton* loadBtn;
    bool bWasConnectedOnce;

signals:
//    void writeToLogSignal(QString);
//    void sendToPort(QString);
//    void setPortNewState(bool);
//    void temperatureIsChanged(QString);
    //void startLoadingConfig();
};

#endif // MAINWINDOW_H
