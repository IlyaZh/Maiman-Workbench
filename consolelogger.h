#ifndef CONSOLELOGGER_H
#define CONSOLELOGGER_H

#include <QObject>
#include <QPlainTextEdit>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QColor>
#include <QDateTime>
#include <QQueue>
#include "commondefines.h"

class consoleLogger : public QObject
{
    Q_OBJECT
public:
    explicit consoleLogger(QString logFileName = DEFAULT_LOG_FILENAME, QObject *parent = 0);
    ~consoleLogger();
    void start();
    void stop();

private:
    QString filePath;
    QString fileName;
    QFile *file;
    QString errorString;
    QQueue<QString> queue;
    void setLogFileName(QString name);
    void setLogFilePath(QString path);

signals:
    void errorOccuredSignal(QString);


public slots:
    //void writeToConsole(QString str, QColor color = QColor::fromRgb(255,255,255,0));
    void writeToLog(QString str);
};

#endif // CONSOLELOGGER_H
