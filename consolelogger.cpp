#include "consolelogger.h"

const qint64 consoleLogger::DAY_TO_REMOVE_LOGS = 5;

consoleLogger::consoleLogger(QString logFileName, QObject *parent) : QObject(parent)
{
    setLogFileName(logFileName);
    setLogFilePath(DEFAULT_LOG_FILEPATH);
    file = new QFile;

    checkAndDeleteOldLogs();

    start();

}

consoleLogger::~consoleLogger() {
    file->close();
    file->deleteLater();
}

void consoleLogger::setLogFileName(QString name) {
    fileName = name;
}

void consoleLogger::setLogFilePath(QString path) {
    filePath = path;
}

void consoleLogger::start() {

    QDir dir = QDir(filePath);
    if(!dir.exists()) {
        dir.mkpath(filePath);
    }

    file->setFileName(filePath + fileName);
    if(!file->open(QIODevice::Append)) {
        emit errorOccuredSignal(file->errorString());
        return;
    }

    writeToLog(" ");
    writeToLog(" ");
    writeToLog("==========================");
    writeToLog("Program start.");
}

void consoleLogger::stop() {
    if(file->isOpen()) file->close();
}

void consoleLogger::writeToLog(QString str) {
    QDateTime timestamp;
    timestamp = QDateTime::currentDateTime();
    QString logString = "[" + timestamp.toString(LOG_TIMESTAMP_DEFAULT_FORMAT) + "] " + str + "\r\n";
    if(file->isOpen()) {
        QTextStream stream(file);

        while(!queue.isEmpty()) {
            stream << queue.dequeue();
        }
        stream << logString;
    } else {
        while(queue.count() >= LOG_MAX_QUEUE_SIZE) {
            queue.dequeue();
        }
        queue.enqueue(logString);
    }
}

void consoleLogger::checkAndDeleteOldLogs() {
    return;
    QDir dir;
    dir.setPath(filePath);
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Time);


    QFileInfoList list = dir.entryInfoList();
    for(int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        if(fileInfo.birthTime().daysTo(QDateTime::currentDateTime()) > DAY_TO_REMOVE_LOGS) {
            QFile::remove(fileInfo.filePath());
        }
    }
}
