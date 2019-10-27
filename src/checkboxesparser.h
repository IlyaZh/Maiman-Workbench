#ifndef CHECKBOXESPARSER_H
#define CHECKBOXESPARSER_H

#include <QObject>
#include <QFile>

class checkboxesParser : public QObject
{
    Q_OBJECT
public:
    explicit checkboxesParser(QString fileName, QObject *parent = nullptr);
    ~checkboxesParser();
    void setFile(QString file);
    void loadData(quint16 deviceId);
    bool hasDataForDevice(quint16 devId);

signals:

public slots:

private:
    QString fileName;
    uint16_t deviceId;
    QFile *file;
};

#endif // CHECKBOXESPARSER_H
