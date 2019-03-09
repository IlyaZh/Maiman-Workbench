#ifndef DEVICE_H
#define DEVICE_H

#include "commondefines.h"
#include "globals.h"
#include <QObject>
#include <QString>
#include <QXmlStreamReader>
#include <QFile>
#include <QMap>
#include <QCheckBox>
#include <QPushButton>

class xmlReader : public QObject
{
    Q_OBJECT
public:
    explicit xmlReader(QObject *parent = nullptr);
    void setConfigFile(QString file);
    void setDeviceOptions(device_t &device, quint16 dev_id);
    void setList(QList<availableDev_t>& listPtr);
    void setBaudsList(QList<uint> &baudList);

private:
    QFile *file;
    //QDomDocument doc;
    QString filePath;
    bool readFile();
    bool parseDevice();
    void parseContent();
    void parseCommand();
    void parseControls();
    void parseLed();
    void parseLedMask();
    void parseParam();
    void parseLimit();
    void parseCalibration();
    void parseButtons();
    void parseSpecialParam();
    QXmlStreamReader xml;
    quint16 deviceId;
    device_t *device;
    QList<availableDev_t>* availableListPtr;
    QList<uint>* bauds;

signals:
    void errorHandler(QString);
    void endOfLoadingConfig(bool);
    void endOfLoadingProgramConfig(bool);


public slots:
    void startLoading();
    void readProgramConfig();


};

#endif // DEVICE_H
