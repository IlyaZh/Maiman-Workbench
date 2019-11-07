#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include "commondefines.h"
#include <QDebug>
#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>

class FileDownloader : public QObject
{
 Q_OBJECT
 public:
  explicit FileDownloader(QUrl fileUrl, QObject *parent = 0);
  virtual ~FileDownloader();
  QByteArray downloadedData();


 signals:
    void downloaded();
    //void needToUpdateSignal(QString);

 private slots:
  void fileDownloaded(QNetworkReply* pReply);

 private:
  QNetworkAccessManager m_WebCtrl;
  QByteArray m_DownloadedData;
  QString url;
};

#endif // FILEDOWNLOADER_H
