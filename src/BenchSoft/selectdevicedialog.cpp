#include "selectdevicedialog.h"
#include "ui_selectdevicedialog.h"

SelectDeviceDialog::SelectDeviceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectDeviceDialog)
{
    ui->setupUi(this);

    connect(this, SIGNAL(accepted()), this, SLOT(acceptedSlot()));
}

SelectDeviceDialog::~SelectDeviceDialog()
{
    delete ui;
}

void SelectDeviceDialog::setList(QStringList list, int selectedIndex) {
    ui->comboBox->clear();
    ui->comboBox->addItems(list);
    if(list.size() > selectedIndex) {
        ui->comboBox->setCurrentIndex(selectedIndex);
    }
}

void SelectDeviceDialog::acceptedSlot() {
    devName = ui->comboBox->currentText();
    emit selectedDevice(devName);
}

void SelectDeviceDialog::setText(QString text) {
    ui->label->setText(text);
}

void SelectDeviceDialog::setTitle(QString title) {
    this->setWindowTitle(title);
}
