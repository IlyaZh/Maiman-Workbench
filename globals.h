#ifndef GLOBALS_H
#define GLOBALS_H

#include <QString>
#include <QMap>
#include <QList>
#include <QCheckBox>
#include <QWidget>
#include "parametercontroller.h"
#include "command.h"
#include "devicelimit.h"

typedef struct {
    QString code;
    QString onCommand;
    QString offCommand;
    quint16 mask;
    QPointer<QPushButton> btnPtr;
} doubleMaskCommand_t;

//typedef struct {
//    QString absMinCode;
//    QString minCode;
//    QString absMaxCode;
//    QString maxCode;
//    QString title;
//    double defaulValue;
//    double divider;
//    QString unit;
//} limits_t;

typedef struct {
    QString title;
    QString code;
    int min;
    int max;
    int divider;
} calibration_t;

typedef struct {
    QString code;
    QString onCommand;
    QString offCommand;
    QString label;
    uint mask;
    QCheckBox* cbPtr;
} binOption_t;

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
    QMap<QString, Command*> commands;
    QList<DeviceLimit*> limits;
    QList<calibration_t> calCoefs;
    QList<ParameterController*> paramWidgets;
    QList<binOption_t> binaryOptions;
    QMap<QString, doubleMaskCommand_t> stateButtons;
    QList<leds_t> leds;
    bool hasLaser;
    bool hasTEC;
    bool startTECwithLaser;
    uint stopCommandDelay;
    uint minCommDelay;
    uint maxCommDelay;
} device_t;

typedef struct {
    QString name;
    quint16 id;
} availableDev_t;

#endif // GLOBALS_H
