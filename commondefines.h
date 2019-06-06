#include <QDebug>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QDate>
#include <QDir>
#include <QSerialPortInfo>
#include <QPointer>
#include <QFont>
#include <QPoint>
//#include "appsettings.h"

#ifndef COMMONDEFINES_H
#define COMMONDEFINES_H

// Название организации
const QString ORG_NAME = "MaimanElectronics";
// Название (заголовок) ПО для служебных путей (без пробелов)
const QString APP_NAME = "BenchSoft";
// Человекопонятное название (заголовок_ ПО
const QString APP_NAME_TITLE = "Maiman BenchSoft";
const QPoint WINDOW_DEFAULT_POSITION = QPoint(-1, -1);
//const QSize WINDOW_DEFAULT_SIZE = QSize(-1,-1);

const QString DEVICE_STATUS_COMMAND = "0700";
const QString TEC_STATUS_COMMAND = "0A1A";

const quint8 MAJOR_VERSION = 1;
const quint8 MINOR_VERSION = 0;
const quint8 PATCH_VERSION = 1;

extern bool debugMode;

const QString SECRET_CODE_COMBINATION = "3.1415926";

const QFont APPLICATION_DEFAULT_FONT(":/fonts/ShareTechMono.ttf", 9);

const QString CONFIG_FILE = QDir::currentPath()+"/DDBconfig.xml";
const QString LOG_TIMESTAMP_DEFAULT_FORMAT = "dd.MM.yyyy HH:mm:ss";
const int LOG_MAX_QUEUE_SIZE = 10;
const unsigned int CONSOLE_MAX_LINES_TO_SHOW = 1000;
const int MAX_FILES_IN_MENU = 5;
const int STATUSBAR_MESSAGE_TIMEOUT = 5000; // ms

// Длительность "покраснения" поля ввода параметров при неверном значении
const int CURR_VALUE_BG_ERROR_TIMEOUT = 1000; // ms

// colors pre-defines
#define LEDS_DEFAULT_COLOR QColor("#171717")
const Qt::GlobalColor LEDS_DEFAULT_ACTIVE_COLOR = Qt::green;
// console test colors
const Qt::GlobalColor CONSOLE_ERROR_COLOR   = Qt::red;
const Qt::GlobalColor CONSOLE_INFO_COLOR    = Qt::yellow;
const Qt::GlobalColor CONSOLE_SEND_COLOR    = Qt::white;
const Qt::GlobalColor CONSOLE_RECEIVE_COLOR = Qt::green;

const QList<QString> bitsName = {"Laser", "TEC", "IntLock", "Error", "Link"};

const char COM_REQUEST_PREFIX = 'J';
const char COM_WRITE_PREFIX = 'P';
const char COM_ANSWER_PREFIX = 'K';
const char COM_ERROR_PREFIX = 'E';
const char COM_END_OF_LINE = '\r';

const QString DEVICE_STOP_COMMAND = QString(COM_WRITE_PREFIX) + "0700 0010";
const QString TEC_STOP_COMMAND = QString(COM_WRITE_PREFIX) + "0A1A 0010";

const unsigned int DEFAULT_BAUD_RATE = 115200;

const unsigned int COM_PORT_TIMEOUT = 1000; // [ms]
const int READ_COM_COMMAND_LENGTH = 11;

const QString DEFAULT_TEMPERATURE_SYMBOL = "C";

const quint16 IDENTIFY_DEVICE_COMMAND = 0x0702;
//enum {IDENTIFY_DEVICE_COMMAND = 0x0702};
const quint16 COMMON_OLD_DEVICES_VALUE = 0x0115;
const unsigned int COM_COMMAND_SEND_DELAY = 150; // ms
const uint COM_COMMAND_MIN_SEND_DELAY = 50;
const uint COM_COMMAND_MAX_SEND_DELAY = 1000;
const quint8 MIN_COM_INTERVAL_COUNTER = 1;
const quint8 MAX_COM_INTERVAL_COUNTER = 100;

const unsigned int STOP_COMMAND_DELAY_DEFAULT = COM_PORT_TIMEOUT-100; // [ms]

enum comErrors {ERROR_BUFFER_OVERLOAD = 0, ERROR_WRONG_COMMAND = 1};
const QList<QString> commErrorsDescription = {"Buffer of device was overloaded", "Device received wrong command"};

const QString DEFAULT_LOG_FILENAME = "consoleLog " + QDate::currentDate().toString("dd.MM.yyyy") + ".log";
const QString DEFAULT_LOG_FILEPATH = QDir::currentPath()+"/logs/";

const int DEFAULT_DIGITS_AFTER_POINT = 2;
#define DEFAULT_FORMAT_FOR_PARAMETERS 'f', DEFAULT_DIGITS_AFTER_POINT
const char DOUBLE_FORMAT = 'f';

inline const QStringList getAvailablePorts() {
    QStringList ports;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
       ports.append(info.portName());
    }
    return ports;
}
inline QString getDefaultPort() { return getAvailablePorts().value(0); }
// Список ID устройств, которые отвечают общим "кодом".
const quint16 DEFAULT_DEV_ID = 16;

enum deviceStatusMask {START_STOP_MASK = 0x2, CURRENT_EXT_INT_MASK = 0x4, START_EXT_INT_MASK = 0x10, BLOCK_THERMO_MASK = 0x40, BLOCK_USE_IGNORE_MASK = 0x80};

const QString HELP_URL = "http://yandex.ru/";
const QString UPDATE_LIST_URL = "http://maiman.online/soft-upd.txt";

enum action {READ = 1, WRITE = 2};
extern QLocale wlocale;
extern QString updateUrl;
const QString appTitle = APP_NAME_TITLE + QString(" V") + QString::number(MAJOR_VERSION) + "." + QString::number(MINOR_VERSION);
extern quint16 devID;


inline double convertCelToFar(double val) {
    return val * 9.0 / 5.0 + 32.0;
}

inline double convertFarToCel(double val) {
    return (val - 32.0) * 5.0 / 9.0;
}

#endif // COMMONDEFINES_H
