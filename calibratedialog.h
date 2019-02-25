#ifndef CalibrateDialog_H
#define CalibrateDialog_H

#include <QDialog>
#include "globals.h"

namespace Ui {
class CalibrateDialog;
}

class CalibrateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CalibrateDialog(QWidget *parent = 0);
    ~CalibrateDialog();
    void setStruct(const calibration_t &itemPtr);

private:
    Ui::CalibrateDialog *ui;
    int coef;
    QString code;
    double divider;

public slots:
    void setValue(double);
    void setValue(int);

private slots:
    void saveResult();

signals:
    void sendData(QString);
};

#endif // CalibrateDialog_H
