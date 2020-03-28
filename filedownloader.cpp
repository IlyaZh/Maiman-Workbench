#include "filedownloader.h"

FileDownloader::FileDownloader(QUrl fileUrl, QObject *parent) :
    QObject(parent)
{
    connect(&m_WebCtrl, SIGNAL (finished(QNetworkReply*)), this, SLOT (fileDownloaded(QNetworkReply*)));
    QNetworkRequest request(fileUrl);
    m_WebCtrl.get(request);
}

FileDownloader::~FileDownloader() { }

void FileDownloader::fileDownloaded(QNetworkReply* pReply) {
    pReply->ignoreSslErrors();
    m_DownloadedData = pReply->readAll();
    //emit a signal
    pReply->deleteLater();

    if(!m_DownloadedData.isEmpty()) emit downloaded();

}

QByteArray FileDownloader::downloadedData() {
    return m_DownloadedData;
}
