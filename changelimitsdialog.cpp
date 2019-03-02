#include "changelimitsdialog.h"
#include "ui_changelimitsdialog.h"

ChangeLimitsDialog::ChangeLimitsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangeLimitsDialog)
{
    ui->setupUi(this);

    connect(ui->okButton, SIGNAL(clicked(bool)), this, SLOT(saveResult()));
    connect(ui->cancelButton, SIGNAL(clicked(bool)), this, SLOT(hide()));
    connect(ui->maxSlider, SIGNAL(valueChanged(int)), this, SLOT(setMax(int)));
    connect(ui->maxValueSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setMax(double)));
    connect(ui->minSlider, SIGNAL(valueChanged(int)), this, SLOT(setMin(int)));
    connect(ui->minValueSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setMin(double)));
}

ChangeLimitsDialog::~ChangeLimitsDialog()
{
    delete ui;
}

void ChangeLimitsDialog::setData(DeviceLimit &devLimit) {
    limit = &devLimit;

    ui->maxValueSpinBox->setDecimals(qRound(log10(limit->getDivider())));
    ui->maxValueSpinBox->setSingleStep(1/limit->getDivider());
    ui->minValueSpinBox->setDecimals(qRound(log10(limit->getDivider())));
    ui->minValueSpinBox->setSingleStep(1/limit->getDivider());

    ui->titleLabel->setText(limit->getTitle() + QString(", ") + limit->getUnit());

    setAbsMin(limit->getBottomValue());
    setAbsMax(limit->getUpperValue());
    setMin(limit->getMinValue());
    setMax(limit->getMaxValue());

    ui->minSlider->setVisible(limit->isShowMin());
    ui->minValueSpinBox->setVisible(limit->isShowMin());
    ui->absMinLabel->setVisible(limit->isShowMin());

    ui->maxSlider->setVisible(limit->isShowMax());
    ui->maxValueSpinBox->setVisible(limit->isShowMax());
    ui->absMaxLabel->setVisible(limit->isShowMax());

    ui->midValueLabel->setVisible(!(limit->isShowMin() && limit->isShowMax()));

//    this->updateValues();
}

void ChangeLimitsDialog::setAbsMax(double val) {
    absMax = val;
    QString formatedValue = QString::number(absMax, DOUBLE_FORMAT, qRound(log10(limit->getDivider()))) + limit->getUnit();
    ui->absMaxLabel->setText(formatedValue);
    ui->maxSlider->setMaximum(qRound(absMax*limit->getDivider()));
    ui->maxValueSpinBox->setMaximum(absMax);
}

void ChangeLimitsDialog::setMax(double val) {
    max = val;
    ui->maxValueSpinBox->setValue(max);
    ui->maxSlider->setValue(qRound(max*limit->getDivider()));

    ui->minSlider->setMaximum(qRound(max*limit->getDivider()));
    ui->minValueSpinBox->setMaximum(max);

    if(!limit->isShowMax()) {
        ui->midValueLabel->setText(QString::number(max, DOUBLE_FORMAT, qRound(log10(limit->getDivider()))) + limit->getUnit());
    }
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

void ChangeLimitsDialog::setMin(double val) {
    min = val;
    ui->minValueSpinBox->setValue(min);
    ui->minSlider->setValue(qRound(min*limit->getDivider()));

    ui->maxSlider->setMinimum(qRound(min*limit->getDivider()));
    ui->maxValueSpinBox->setMinimum(min);

    if(!limit->isShowMin()) {
        ui->midValueLabel->setText(QString::number(min, DOUBLE_FORMAT, qRound(log10(limit->getDivider()))) + limit->getUnit());
    }
}

void ChangeLimitsDialog::setMin(int val) {
    min = static_cast<double> (val)/limit->getDivider();
    this->setMin(min);
}

//void ChangeLimitsDialog::updateValues() {
//    setAbsMax(absMax);
//    setAbsMin(absMin);
//    setMin(min);
//    setMax(max);
//}

void ChangeLimitsDialog::saveResult() {
    if(limit->isShowMin() && !limit->getMinCode().isEmpty()) {
        int value = ui->minSlider->value();
        QString strToSend = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(limit->getMinCode()).arg(value, 4, 16, QChar('0')).toUpper();
        emit sendData(strToSend);
    }

    if(limit->isShowMax() && !limit->getMaxCode().isEmpty()) {
        int value = ui->maxSlider->value();
        QString strToSend = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(limit->getMaxCode()).arg(value, 4, 16, QChar('0')).toUpper();
        emit sendData(strToSend);
    }
    this->hide();

}
