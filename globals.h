#ifndef GLOBALS_H
#define GLOBALS_H

#include <QString>
#include <QMap>
#include <QList>
#include <QCheckBox>
#include <QWidget>
#include "parameterform.h"
#include "parameter_t.h"

/*typedef struct {
    QString Title;
    QString Name;
    quint8 Action;
} paramBit_t;*/

/*typedef struct {
    QString Code;
    quint8 Action;
    int Divider;
    bool isTemperature;
    union {
        double dValue;
        uint16_t iValue;
    };
} parameter_t;*/


typedef struct {
    QString code;
    QString onCommand;
    QString offCommand;
    quint16 mask;
    QPointer<QPushButton> btnPtr;
} doubleMaskCommand_t;

typedef struct {
    QString absMinCode;
    QString minCode;
    QString absMaxCode;
    QString maxCode;
    QString title;
    double defaulValue;
//    double absMinValue;
//    double absMaxValue;
//    double minValue;
//    double maxValue;
    double divider;
    QString unit;
} limits_t;

typedef struct {
    QString title;
    QString code;
    int min;
    int max;
    double divider;
} calibration_t;

typedef struct {
    uint8_t id;
    QString code;
    QString onCommand;
    QString offCommand;
    QString label;
    uint mask;
    QCheckBox* cbPtr;
} specParams_t;

/*typedef struct {
    QString unit;
    QString minComm;
    QString maxComm;
    QString valueComm;
    QString realComm;
    double divider;
    bool isTemperatureFlag;
    QString title;
} parameterTemp_t;*/

typedef struct {
    QString command;
    quint16 mask;
    QColor activeColor;
    QColor defaultColor;
    QString message;
} ledMask_t;

typedef struct {
    QString label;
    QList<ledMask_t> masks;
} leds_t;

typedef struct {
    QString devName;
    QString image;
    QString description;
    QString link;
    quint16 deviceID;
    QList<parameter_t*> commands;
    QList<limits_t> limits;
    QList<calibration_t> calibrate;
    QList<ParameterForm*> paramWidgets;
    //QMap<QString, QString> showInWindowParameters;
    QList<specParams_t> specialParameters;
    QMap<QString, doubleMaskCommand_t> stateButtons;
    QList<leds_t> leds;
    bool hasLaser;
    bool hasTEC;
    bool startTECwithLaser;
    uint stopCommandDelay;
    int minCommDelay;
    int maxCommDelay;
} device_t;

typedef struct {
    QString name;
    quint16 id;
} availableDev_t;

//extern QList<device_t> availableDevices;

#endif // GLOBALS_H
