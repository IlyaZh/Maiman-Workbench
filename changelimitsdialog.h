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
    explicit ChangeLimitsDialog(QWidget *parent = nullptr);
    void setData(DeviceLimit *devLimit);
    ~ChangeLimitsDialog();

private:
    Ui::ChangeLimitsDialog *ui;
    DeviceLimit *limit;
    void setMax();
    void setMin();

public slots:
    void saveResult();

signals:
    void sendData(QString commStr);

};

#endif // CHANGELIMITSDIALOG_H
