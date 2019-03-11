#include "commondefines.h"
#include "mainwindow.h"
#include "comportsettingsdialog.h"
#include "comport.h"
#include "globals.h"
#include <QApplication>
#include <QFontDatabase>
#include <QCommandLineParser>
#include <QMessageBox>

QLocale wlocale;
QString updateUrl;
//QPointer<QSettings> qsettings = new QSettings(QSettings::NativeFormat, QSettings::UserScope, ORG_NAME, APP_NAME);
//quint16 devID = 0;
AppSettings settings;
bool debugMode = false;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationVersion(QString::number(MAJOR_VERSION) + "." + QString::number(MINOR_VERSION) + "." + QString::number(PATCH_VERSION));
    QApplication::setFont(APPLICATION_DEFAULT_FONT);

    QCommandLineParser cliParser;
    QCommandLineOption debugOption(QStringList() << "d" << "debug");
    cliParser.addOption(debugOption);
    if(cliParser.isSet(debugOption)) {
        debugMode = true;
    }

    wlocale = QLocale(QLocale::system());

    updateUrl.clear();

    // Определение экземпляров окон программы
    MainWindow w;
    w.setFont(APPLICATION_DEFAULT_FONT);

    // Отобразить главное окно
    w.show();

    return a.exec();
}
