#ifndef ABOUTDEVICEDIALOG_H
#define ABOUTDEVICEDIALOG_H

#include <QDialog>
#include <QImageReader>
#include "commondefines.h"
#include "globals.h"

extern AppSettings settings;

namespace Ui {
class AboutDeviceDialog;
}

class AboutDeviceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDeviceDialog(QWidget *parent = 0);
    ~AboutDeviceDialog();
    void setInfo(const device_t& device);

private:
    Ui::AboutDeviceDialog *ui;
//    const device_t* dev;

private slots:
    void saveDataAndClose();
};

#endif // ABOUTDEVICEDIALOG_H
