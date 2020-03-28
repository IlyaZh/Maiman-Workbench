#include "comportsettingsdialog.h"
#include "ui_comportsettingsdialog.h"


ComPortSettingsDialog::ComPortSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ComPortSettingsDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("COM port settings window"));
    qsettings = new QSettings(QSettings::NativeFormat, QSettings::UserScope, ORG_NAME, APP_NAME);
    connect(ui->refreshAvailableComPorts, SIGNAL(released()), this, SLOT(preloadData()));
    preloadData();
}

ComPortSettingsDialog::~ComPortSettingsDialog()
{
    delete ui;
}

void ComPortSettingsDialog::preloadData() {
    ui->selectPort->clear();
    ui->selectBaudRate->clear();

    ui->selectPort->addItems(getAvailablePorts());
    QStringList ports;
    foreach (const qint32 val, comBaudRates) {
        ports.append(QString::number(val));
    }
    ui->selectBaudRate->addItems(ports);

    ui->selectBaudRate->setCurrentText(qsettings->value("userSettings/comPort/baudRate", DEFAULT_BAUD_RATE).toString());
    ui->selectPort->setCurrentText(qsettings->value("userSettings/comPort/port", DEFAULT_PORT).toString());
}

void ComPortSettingsDialog::accept() {
    qsettings->setValue("userSettings/comPort/baudRate", ui->selectBaudRate->currentText());
    qsettings->setValue("userSettings/comPort/port", ui->selectPort->currentText());

    emit comportSettingsHasChangedSignal();
    this->hide();
}

void ComPortSettingsDialog::reject() {
    this->hide();
}
