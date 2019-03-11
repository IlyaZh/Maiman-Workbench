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

void CalibrateDialog::setStruct(const calibration_t &itemPtr) {
    code = itemPtr.code;
    divider = itemPtr.divider;

    ui->titleLabel->setText(itemPtr.title);
    QString desc = QString("Min: %1, Max: %2").arg(QString::number(itemPtr.min)).arg(QString::number(itemPtr.max));
    ui->valueSlider->setMinimum(itemPtr.min);
    ui->valueSlider->setMaximum(itemPtr.max);
    ui->valueDoubleSpinBox->setDecimals(qRound(log10(itemPtr.divider)));
    ui->valueDoubleSpinBox->setMinimum(static_cast<double>(itemPtr.min)/itemPtr.divider);
    ui->valueDoubleSpinBox->setMaximum(static_cast<double>(itemPtr.max)/itemPtr.divider);

//    qDebug() << itemPtr.min << itemPtr.max << log10(itemPtr.divider) << (double)itemPtr.min/itemPtr.divider << (double)itemPtr.max/itemPtr.divider;
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
//    qDebug() << coef << value;
}

void CalibrateDialog::saveResult() {
    int value = ui->valueSlider->value();
    QString strToSend = QString("%1%2 %3").arg(COM_WRITE_PREFIX).arg(code).arg(value, 4, 16, QChar('0')).toUpper();
    emit sendData(strToSend);
}


