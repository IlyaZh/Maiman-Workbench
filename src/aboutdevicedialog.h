#ifndef ABOUTDEVICEDIALOG_H
#define ABOUTDEVICEDIALOG_H

#include <QDialog>
#include <QImageReader>
#include "commondefines.h"
#include "globals.h"
#include "appsettings.h"

namespace Ui {
class AboutDeviceDialog;
}

class AboutDeviceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDeviceDialog(AppSettings *settings, QWidget *parent = nullptr);
    ~AboutDeviceDialog();
    void setInfo(const device_t& device);

private:
    Ui::AboutDeviceDialog *ui;
    AppSettings* settings;

private slots:
    void saveDataAndClose();
};

#endif // ABOUTDEVICEDIALOG_H
