#include "commondefines.h"
#include "mainwindow.h"
#include "comportsettingsdialog.h"
#include "comport.h"
#include "globals.h"
#include <QApplication>
#include <QFontDatabase>

QLocale wlocale;
QString updateUrl;
//QPointer<QSettings> qsettings = new QSettings(QSettings::NativeFormat, QSettings::UserScope, ORG_NAME, APP_NAME);
quint16 devID = 0;
AppSettings settings;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setFont(APPLICATION_DEFAULT_FONT);

    wlocale = QLocale(QLocale::system());

    updateUrl.clear();

    // Определение экземпляров окон программы
    MainWindow w;
    w.setFont(APPLICATION_DEFAULT_FONT);

    // Отобразить главное окно
    w.show();

    return a.exec();
}
