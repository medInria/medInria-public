/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medFirstStartDownloader.h>

#include <QThread>



#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>

#include <QMap>
#include <QPair>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
class medHttpFirstStartRecever : public QObject
{
public:
    medHttpFirstStartRecever();
    ~medHttpFirstStartRecever();

    void slotDownloadProgress(qint64 bytesSent, qint64 bytesTotal);
    void currentResponse(QNetworkReply *reply, int httpVerb, int status, qint64 bytesSent, qint64 bytesTotal);
    void slotFinished();
    void slotError(QNetworkReply::NetworkError err);
};

class medFirstStartPrivate : public QObject
{
public:
    medFirstStartPrivate() = default;
    ~medFirstStartPrivate() = default;

    
    QMap <QNetworkReply *, QString> networkReplyMap;

    QMap <QString, QPair <QString, QUrl> > registerdMap;
    QMap <QString, QPair <QString, QUrl> > updateMap;
    QMap <QString, QPair <QString, QUrl> > updatedMap;

    QString baseUpdatedPath;
    QString confiPath;

    medHttpFirstStartRecever recever;
};

medFirstStartDownloader::medFirstStartDownloader()
{
    //d = new medFirstStartPrivate();
    //d->baseUpdatedPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/" + QCoreApplication::organizationName() + "/" + QCoreApplication::applicationName() + "/resources/conf/";
    //d->confiPath = "updatedBaseConfigs.json";
}

medFirstStartDownloader::~medFirstStartDownloader()
{
}

void medFirstStartDownloader::askNewVersion()
{
    bool bRes = true;

    for (auto const & key : m_downloadMap.keys())
    {
        auto & url = m_downloadMap[key].second;
        if (!url.isEmpty())
        {
            QNetworkRequest request(url);
            QNetworkReply * reply = nullptr; // m_qnam.get(request);

            // Connecting the request manager slots to the signals sent by the reply object
            QObject::connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(slotDownloadProgress(qint64, qint64)));
            QObject::connect(reply, SIGNAL(finished()), this, SLOT(slotFinished()));
            QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
        }
    }
}

void medFirstStartDownloader::setDownload(QMap<QString, QPair<QString, QUrl>> downloadMap)
{
    m_downloadMap = downloadMap;
}

QMap<QString, QPair<QString, QUrl>> medFirstStartDownloader::getDownloadedMap()
{
    return m_downloadedMap;
}

QStringList medFirstStartDownloader::getDownloadFailed()
{
    return m_downloadFailed;
}





//--------------------------------------------------------------------------------------------------------------------------

void medFirstStartDownloader::slotDownloadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    // retrieving the reply object who sent a download signal
    QNetworkReply *reply = dynamic_cast<QNetworkReply*>(QObject::sender());
    //TODO Log


}

void medFirstStartDownloader::currentResponse(QNetworkReply *reply, int httpVerb, int status, qint64 bytesSent, qint64 bytesTotal)
{
    // the response is in progress, therefore, the payload is empty. 
    QByteArray payload = QByteArray();
    // storing the bytes information of the reply in the header
    QList<QPair<QByteArray, QByteArray>> headers;

    // sending the signal for the network classes to handle the reply information 

}

void medFirstStartDownloader::slotFinished()
{
    // retrieving the reply object who sent a finish signal
    QNetworkReply *reply = dynamic_cast<QNetworkReply*>(QObject::sender());

    //handling the reply finish information
    if (reply)
    {
        auto payload = reply->readAll();
        auto headers = reply->rawHeaderPairs();
    }
}

void medFirstStartDownloader::slotError(QNetworkReply::NetworkError err)
{
    // retrieving the reply object who sent an error signal
    QNetworkReply *reply = dynamic_cast<QNetworkReply*>(QObject::sender());
    //handling the reply error information
    if (reply)
    {
        int httpCode = static_cast<int>(reply->error());
        //TODO log error and insert in failed list
    }
}


