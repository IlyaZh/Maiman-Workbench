#ifndef COMPORTSETTINGSDIALOG_H
#define COMPORTSETTINGSDIALOG_H

#include "commondefines.h"
#include <QDialog>
#include <QSettings>
#include "comport.h"

namespace Ui {
class ComPortSettingsDialog;
}

class ComPortSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ComPortSettingsDialog(QWidget *parent = 0);
    ~ComPortSettingsDialog();

private:
    Ui::ComPortSettingsDialog *ui;

    QSettings *qsettings;

private slots:
    void accept();
    void reject();
    void preloadData();

signals:
    void comportSettingsHasChangedSignal();

};

#endif // COMPORTSETTINGSDIALOG_H
