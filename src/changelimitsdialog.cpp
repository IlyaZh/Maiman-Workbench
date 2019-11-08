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

    ui->valueSlider->setValue(limit->getLimitRaw());
    ui->valueSpinBox->setValue(limit->getLimitValue());
    qDebug() << limit->getLimitValue();

//    this->updateValues();
}

void ChangeLimitsDialog::setMax() {
    ui->valueSlider->setMaximum(limit->getMaxRaw());
    ui->valueSpinBox->setMaximum(limit->getMaxValue());
    ui->maxLabel->setText(QString::number(limit->getMaxValue()));
}

void ChangeLimitsDialog::setMin() {
    ui->valueSlider->setMinimum(limit->getMinRaw());
    ui->valueSpinBox->setMinimum(limit->getMinValue());
    ui->minLabel->setText(QString::number(limit->getMinValue()));
}
/*
void ChangeLimitsDialog::setAbsMax(double val) {
    absMax = val;
    QString formatedValue = QString::number(absMax, DOUBLE_FORMAT, qRound(log10(limit->getDivider()))) + limit->getUnit();
    ui->absMaxLabel->setText(formatedValue);
    ui->maxSlider->setMaximum(qRound(absMax*limit->getDivider()));
    ui->maxValueSpinBox->setMaximum(absMax);
}



void ChangeLimitsDialog::setMax(int val) {
    max = static_cast<double> (val)/limit->getDivider();
    this->setMax(max);
}

void ChangeLimitsDialog::setAbsMin(double val) {
    absMin = val;
    QString formatedValue = QString::number(absMin, DOUBLE_FORMAT, qRound(log10(limit->getDivider()))) + limit->getUnit();
    ui->absMinLabel->setText(formatedValue);
    ui->minSlider->setMinimum(qRound(absMin*limit->getDivider()));
    ui->minValueSpinBox->setMinimum(absMin);
}


void ChangeLimitsDialog::setMin(int val) {
    min = static_cast<double> (val)/limit->getDivider();
    this->setMin(min);
}*/

void ChangeLimitsDialog::saveResult() {
        int value = ui->valueSlider->value();
        QString strToSend = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(limit->getLimitCode()).arg(value, 4, 16, QChar('0')).toUpper();
        emit sendData(strToSend);
    this->hide();

}

