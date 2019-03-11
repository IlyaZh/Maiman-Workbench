#include "calibratedialog.h"
#include "ui_calibratedialog.h"

CalibrateDialog::CalibrateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CalibrateDialog)
{
    ui->setupUi(this);
    coef = 0;
    connect(ui->valueSlider, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
    connect(ui->valueDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setValue(double)));
    connect(this, SIGNAL(accepted()), this, SLOT(saveResult()));
}

CalibrateDialog::~CalibrateDialog()
{
    delete ui;
}

void CalibrateDialog::setStruct(const calibration_t &calibrationData) {
    code = calibrationData.code;
    divider = calibrationData.divider;

    ui->titleLabel->setText(calibrationData.title);
    QString desc = QString("Min: %1, Max: %2").arg(QString::number(calibrationData.min)).arg(QString::number(calibrationData.max));
    ui->valueSlider->setMinimum(calibrationData.min);
    ui->valueSlider->setMaximum(calibrationData.max);
    ui->valueSlider->setSingleStep(1.0/calibrationData.divider);
    ui->valueDoubleSpinBox->setSingleStep(1.0/calibrationData.divider);
    ui->valueDoubleSpinBox->setDecimals(qRound(log10(calibrationData.divider)));
    ui->valueDoubleSpinBox->setMinimum(static_cast<double>(calibrationData.min)/calibrationData.divider);
    ui->valueDoubleSpinBox->setMaximum(static_cast<double>(calibrationData.max)/calibrationData.divider);
}

void CalibrateDialog::setValue(int value) {
    coef = value;
    ui->valueSlider->setValue(coef);
    ui->valueDoubleSpinBox->setValue(qRound(static_cast<double>(coef)/divider));
}

void CalibrateDialog::setValue(double value) {
    coef = qRound(value*divider);
    ui->valueSlider->setValue(coef);
    ui->valueDoubleSpinBox->setValue(value);
}

void CalibrateDialog::saveResult() {
    int value = ui->valueSlider->value();
    QString strToSend = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(code).arg(value, 4, 16, QChar('0')).toUpper();
    emit sendData(strToSend);
}


