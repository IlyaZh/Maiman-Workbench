#include "calibratedialog.h"
#include "ui_calibratedialog.h"

CalibrateDialog::CalibrateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CalibrateDialog)
{
    ui->setupUi(this);

    connect(ui->valueSlider, &QSlider::valueChanged, [=](int value){
        ui->valueSlider->setValue(value);
        ui->valueDoubleSpinBox->setValue((static_cast<double>(value)/code->getDivider()));
    });
    connect(ui->valueDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
          [=](double value){
        ui->valueSlider->setValue(static_cast<quint16>(qRound(value*code->getDivider())));
        ui->valueDoubleSpinBox->setValue(value);
    });
    connect(this, SIGNAL(accepted()), this, SLOT(saveResult()));
}

CalibrateDialog::~CalibrateDialog()
{
    delete ui;
}

void CalibrateDialog::setStruct(const calibration_t &calibrationData) {
    code = calibrationData.cmd;

    ui->titleLabel->setText(calibrationData.title);
    QString desc = QString("Min: %1, Max: %2").arg(QString::number(calibrationData.min)).arg(QString::number(calibrationData.max));
    ui->valueSlider->setMinimum(calibrationData.min);
    ui->valueSlider->setMaximum(calibrationData.max);
    ui->valueDoubleSpinBox->setSingleStep(1.0/code->getDivider());
    ui->valueDoubleSpinBox->setDecimals(qRound(0.43429*qLn(code->getDivider())));
    ui->valueDoubleSpinBox->setMinimum(static_cast<double>(calibrationData.min)/code->getDivider());
    ui->valueDoubleSpinBox->setMaximum(static_cast<double>(calibrationData.max)/code->getDivider());
}

void CalibrateDialog::saveResult() {
    int value = ui->valueSlider->value();
    QString strToSend = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(code->getCode()).arg(value, 4, 16, QChar('0')).toUpper();
    code->resetInterval();
    emit sendData(strToSend);
}


