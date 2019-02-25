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

void ChangeLimitsDialog::setStruct(const limits_t &item) {
    commands = &item;

    ui->maxValueSpinBox->setDecimals(qRound(log10(commands->divider)));
    ui->maxValueSpinBox->setSingleStep(1/commands->divider);
    ui->minValueSpinBox->setDecimals(qRound(log10(commands->divider)));
    ui->minValueSpinBox->setSingleStep(1/commands->divider);

    ui->titleLabel->setText(commands->title + QString(", ") + commands->unit);

    ui->minSlider->setVisible(!item.absMinCode.isEmpty());
    ui->minValueSpinBox->setVisible(!item.absMinCode.isEmpty());
    ui->absMinLabel->setVisible(!item.absMinCode.isEmpty());

    ui->maxSlider->setVisible(!item.absMaxCode.isEmpty());
    ui->maxValueSpinBox->setVisible(!item.absMaxCode.isEmpty());
    ui->absMaxLabel->setVisible(!item.absMaxCode.isEmpty());

    ui->midValueLabel->setVisible(item.absMinCode.isEmpty() || item.absMaxCode.isEmpty());

//    this->updateValues();
}

void ChangeLimitsDialog::setAbsMax(double val) {
    absMax = val;
    QString formatedValue = QString::number(absMax, DOUBLE_FORMAT, log10(commands->divider)) + commands->unit;
    ui->absMaxLabel->setText(formatedValue);
    ui->maxSlider->setMaximum(qRound(absMax*commands->divider));
    ui->maxValueSpinBox->setMaximum(absMax);
}

void ChangeLimitsDialog::setMax(double val) {
    max = val;
    ui->maxValueSpinBox->setValue(max);
    ui->maxSlider->setValue(qRound(max*commands->divider));

    ui->minSlider->setMaximum(qRound(max*commands->divider));
    ui->minValueSpinBox->setMaximum(max);

    if(commands->absMaxCode.isEmpty()) {
        ui->midValueLabel->setText(QString::number(max, DOUBLE_FORMAT, log10(commands->divider)) + commands->unit);
    }
}

void ChangeLimitsDialog::setMax(int val) {
    max = (double)val/commands->divider;
    this->setMax(max);
}

void ChangeLimitsDialog::setAbsMin(double val) {
    absMin = val;
    QString formatedValue = QString::number(absMin, DOUBLE_FORMAT, log10(commands->divider)) + commands->unit;
    ui->absMinLabel->setText(formatedValue);
    ui->minSlider->setMinimum(qRound(absMin*commands->divider));
    ui->minValueSpinBox->setMinimum(absMin);
}

void ChangeLimitsDialog::setMin(double val) {
    min = val;
    ui->minValueSpinBox->setValue(min);
    ui->minSlider->setValue(qRound(min*commands->divider));

    ui->maxSlider->setMinimum(qRound(min*commands->divider));
    ui->maxValueSpinBox->setMinimum(min);

    if(commands->absMinCode.isEmpty()) {
        ui->midValueLabel->setText(QString::number(min, DOUBLE_FORMAT, log10(commands->divider)) + commands->unit);
    }
}

void ChangeLimitsDialog::setMin(int val) {
    min = (double)val/commands->divider;
    this->setMin(min);
}

//void ChangeLimitsDialog::updateValues() {
//    setAbsMax(absMax);
//    setAbsMin(absMin);
//    setMin(min);
//    setMax(max);
//}

void ChangeLimitsDialog::saveResult() {
    if(!commands->absMinCode.isEmpty()) {
        int value = ui->minSlider->value();
        QString strToSend = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(commands->minCode).arg(value, 4, 16, QChar('0')).toUpper();
        emit sendData(strToSend);
    }

    if(!commands->absMaxCode.isEmpty()) {
        int value = ui->maxSlider->value();
        QString strToSend = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(commands->maxCode).arg(value, 4, 16, QChar('0')).toUpper();
        emit sendData(strToSend);
    }
    this->hide();

}
