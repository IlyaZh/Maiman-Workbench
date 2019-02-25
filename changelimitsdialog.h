#ifndef CHANGELIMITSDIALOG_H
#define CHANGELIMITSDIALOG_H

#include <QDialog>
#include "commondefines.h"
#include "globals.h"
#include <QString>

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
    const limits_t *commands;

public slots:
    void setStruct(const limits_t&);
    void setAbsMax(double val);
    void setMax(double val);
    void setMax(int val);
    void setAbsMin(double val);
    void setMin(double val);
    void setMin(int val);
//    void updateValues();
    void saveResult();

signals:
    void sendData(QString commStr);

};

#endif // CHANGELIMITSDIALOG_H
