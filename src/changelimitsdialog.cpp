#include "changelimitsdialog.h"
#include "ui_changelimitsdialog.h"
#include "math.h"

ChangeLimitsDialog::ChangeLimitsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangeLimitsDialog)
{
    ui->setupUi(this);

    connect(ui->okButton, SIGNAL(clicked(bool)), this, SLOT(saveResult()));
    connect(ui->cancelButton, SIGNAL(clicked(bool)), this, SLOT(hide()));
    connect(ui->slider, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
    connect(ui->valueSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setValue(double)));
}

ChangeLimitsDialog::~ChangeLimitsDialog()
{
    delete ui;
}

void ChangeLimitsDialog::setData(DeviceLimit *devLimit) {
    limit = devLimit;

    ui->valueSpinBox->setDecimals(qRound(log10(limit->getDivider())));
    ui->valueSpinBox->setSingleStep(1/limit->getDivider());

    ui->titleLabel->setText(limit->getTitle() + QString(", ") + limit->getUnit());

    setMin();
    setMax();

    ui->slider->setValue(qRound(limit->getValue()*limit->getDivider()));
    ui->valueSpinBox->setValue(limit->getValue());

}

void ChangeLimitsDialog::setMin() {
    ui->valueSpinBox->setMinimum(limit->getMin());
    ui->slider->setMinimum(qRound(limit->getMin()*limit->getDivider()));
    QString formatedValue = QString::number(limit->getMin(), DOUBLE_FORMAT, qRound(log10(limit->getDivider()))) + limit->getUnit();
    ui->maxLabel->setText(formatedValue);
}

void ChangeLimitsDialog::setMax() {
    ui->valueSpinBox->setMaximum(limit->getMax());
    ui->slider->setMaximum(qRound(limit->getMax()*limit->getDivider()));
    QString formatedValue = QString::number(limit->getMax(), DOUBLE_FORMAT, qRound(log10(limit->getDivider()))) + limit->getUnit();
    ui->maxLabel->setText(formatedValue);
}

void ChangeLimitsDialog::setValue(double value) {
    ui->slider->setValue(qRound(value*limit->getDivider()));
    ui->valueSpinBox->setValue(value);
}

void ChangeLimitsDialog::setValue(int value) {
    ui->slider->setValue(value);
    ui->valueSpinBox->setValue(value/limit->getDivider());
}

void ChangeLimitsDialog::saveResult() {
        QString strToSend = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(limit->getCode()).arg(ui->slider->value(), 4, 16, QChar('0')).toUpper();
        emit sendData(strToSend);
    this->hide();

}
