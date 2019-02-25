#ifndef DEVICE_H
#define DEVICE_H

#include "commondefines.h"
#include "globals.h"
#include <QObject>
#include <QString>
#include <QXmlStreamReader>
//#include <QXmlStreamWriter>
#include <QFile>
//#include <QStandardItemModel>
#include <QMap>
#include <QCheckBox>
#include <QPushButton>
//#include <QDomDocument>
//#include <QDomElement>
//#include <QDomNode>

#include "globals.h"

class xmlReader : public QObject
{
    Q_OBJECT
public:
    explicit xmlReader(QObject *parent = nullptr);
    ~xmlReader();
    void setConfigFile(QString file);
    void setDeviceOptions(device_t &dev, quint16 dev_id);
    void setList(QList<availableDev_t>& listPtr);
    void setBaudsList(QList<uint> &baudList);
//    QVector<quint16> &getAvailableDevices();

private:
    QFile *file;
    //QDomDocument doc;
    QString filePath;
    bool readFile();
    bool parseDevice();
    void parseContent();
    void parseCommand();
    void parseControls();
//    void parseLeds();
    void parseLed();
    void parseLedMask();
    void parseParam();
    void parseLimit();
    void parseCalibration();
    void parseButtons();
    //void parseInWindowItems();
    void parseSpecialParam();
    QXmlStreamReader xml;
    quint16 deviceId;
    device_t *dev;
//    int currDevIndex;
//    bool currDeviceFound;
    QList<availableDev_t>* availableListPtr;
    QList<uint>* bauds;
//    int lastLed;
//    QList<parameterTemp_t> tempParams;

signals:
    void errorHandler(QString);
    void endOfLoadingConfig(bool);
    void endOfLoadingProgramConfig(bool);


public slots:
    void startLoading();
    void readProgramConfig();


};

#endif // DEVICE_H
