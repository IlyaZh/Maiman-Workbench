#ifndef CalibrateDialog_H
#define CalibrateDialog_H

#include <QDialog>
#include "globals.h"
#include "command.h"

namespace Ui {
class CalibrateDialog;
}

class CalibrateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CalibrateDialog(QWidget *parent = nullptr);
    ~CalibrateDialog();
    void setStruct(const calibration_t &itemPtr);

private:
    Ui::CalibrateDialog *ui;
    Command* code;
    int divider;

private slots:
    void saveResult();

signals:
    void sendData(QString);
};

#endif // CalibrateDialog_H
