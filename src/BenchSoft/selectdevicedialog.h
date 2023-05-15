#ifndef SELECTDEVICEDIALOG_H
#define SELECTDEVICEDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class SelectDeviceDialog;
}

class SelectDeviceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectDeviceDialog(QWidget *parent = 0);
    ~SelectDeviceDialog();

signals:
    void selectedDevice(QString);

private slots:
    void acceptedSlot();

public slots:
    void setList(QStringList list, int selectedIndex = 0);
    void setText(QString);
    void setTitle(QString);

private:
    Ui::SelectDeviceDialog *ui;
    QString devName;
};

#endif // SELECTDEVICEDIALOG_H
