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
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(saveResult()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(hide()));
}

CalibrateDialog::~CalibrateDialog()
{
    delete ui;
}

void CalibrateDialog::setStruct(const calibration_t &calibrationData) {
    code = calibrationData.cmd;

    ui->titleLabel->setText(calibrationData.title);
    ui->valueSlider->setMinimum(qRound(calibrationData.min*code->getDivider()));
    ui->valueSlider->setMaximum(qRound(calibrationData.max*code->getDivider()));
    ui->valueDoubleSpinBox->setSingleStep(1.0/code->getDivider());
    int precision = qRound(0.43429*qLn(code->getDivider()));
    ui->valueDoubleSpinBox->setDecimals(precision);

    ui->valueDoubleSpinBox->setMinimum(calibrationData.min);
    ui->valueDoubleSpinBox->setMaximum(calibrationData.max);

    ui->minLabel->setText(wlocale.toString(calibrationData.min, DOUBLE_FORMAT, precision));
    ui->maxLabel->setText(wlocale.toString(calibrationData.max, DOUBLE_FORMAT, precision));

    ui->valueSlider->setValue(qRound(code->getValue()*code->getDivider()));
    ui->valueDoubleSpinBox->setValue(code->getValue());
}

void CalibrateDialog::saveResult() {
    int value = ui->valueSlider->value();
    QString strToSend = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(code->getCode()).arg(static_cast<quint16>(value), 4, 16, QChar('0')).toUpper();
    code->resetInterval();
    emit sendData(strToSend);
    this->hide();
}


