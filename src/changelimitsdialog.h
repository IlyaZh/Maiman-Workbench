#ifndef CHANGELIMITSDIALOG_H
#define CHANGELIMITSDIALOG_H

#include <QDialog>
#include "commondefines.h"
#include "globals.h"
#include <QString>
#include "devicelimit.h"

namespace Ui {
class ChangeLimitsDialog;
}

class ChangeLimitsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeLimitsDialog(QWidget *parent = 0);
    ~ChangeLimitsDialog();

private:
    Ui::ChangeLimitsDialog *ui;
    double min, max, absMin, absMax;
    DeviceLimit *limit;

public slots:
    void setData(DeviceLimit*);
    void saveResult();

private slots:
    void setMax();
    void setMin();
    void setValue(double val);
    void setValue(int val);

signals:
    void sendData(QString commStr);

};

#endif // CHANGELIMITSDIALOG_H
