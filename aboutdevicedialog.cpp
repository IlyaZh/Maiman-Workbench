#include "aboutdevicedialog.h"
#include "ui_aboutdevicedialog.h"

AboutDeviceDialog::AboutDeviceDialog(AppSettings* appSettings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDeviceDialog),
    settings(appSettings)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/logo-minimal.png"));
    connect(ui->cancelButton, SIGNAL(clicked(bool)), this, SLOT(hide()));
    connect(ui->okButton, SIGNAL(clicked(bool)), this, SLOT(saveDataAndClose()));

}

AboutDeviceDialog::~AboutDeviceDialog()
{
    delete ui;
}

void AboutDeviceDialog::setInfo(const device_t& device) {
    QString filepath = "devices" + QString(QDir::separator()) + device.image;
    QPixmap devImg(filepath);

//    devImg = devImg.scaled(200, 200, Qt::KeepAspectRatio);
    ui->imageLabel->setPixmap(devImg);

    ui->nameLabel->setText(device.devName);

    ui->descriptionLabel->setText(device.description);
    QString link = QString("");
    if(!device.link.isEmpty())
        link = QString("<a href=\"%1\" style=\"color: #fff;\">Product page</a>").arg(device.link);
    ui->linkLabel->setText(link);

    if(device.hasLaser) {
        ui->hasLaserLabel->setText("Laser is available");
    } else {
        ui->hasLaserLabel->setText("Laser is unavailable");
    }

    if(device.hasTEC) {
        ui->hasTECLabel->setText("TEC is available");
    } else {
        ui->hasTECLabel->setText("TEC is unavailable");
    }

    ui->comPortDelaySpinBox->setMinimum(device.minCommDelay);
    ui->comPortDelaySpinBox->setMaximum(device.maxCommDelay);
    ui->comPortDelaySpinBox->setValue(qBound(device.minCommDelay, settings->getComCommandsDelay(), device.maxCommDelay));
    QString limitsStr = QString("From %1 to %2 ms").arg(QString::number(device.minCommDelay)).arg(QString::number(device.maxCommDelay));
    ui->comPortDelayLimitsLabel->setText(limitsStr);

}

void AboutDeviceDialog::saveDataAndClose() {
    settings->setComCommandsDelay(ui->comPortDelaySpinBox->value());
    this->hide();
}
