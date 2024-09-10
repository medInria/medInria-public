/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medFirstStartDownloader.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

#include <QMap>
#include <QPair>

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>

class medFirstStartDownloaderPrivate
{
public:
    medFirstStartDownloaderPrivate() = default;
    ~medFirstStartDownloaderPrivate() = default;

    QMap<QNetworkReply*, QEventLoop*> replyWaiterMap;
    QMap<QNetworkReply*, QString> replyPathMap;
    QNetworkAccessManager * qnam;
};

medFirstStartDownloader::medFirstStartDownloader(QNetworkAccessManager * qnam)
{
    d = new medFirstStartDownloaderPrivate();
    d->qnam = qnam;
}

medFirstStartDownloader::~medFirstStartDownloader()
{
    delete d;
}



bool medFirstStartDownloader::download(QString url, QString path)
{
    bool bRes = false;

    if (!url.isEmpty() && !path.isEmpty())
    {
        QNetworkRequest request(url);
        request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
        QNetworkReply * reply = d->qnam->get(request);

        // Connecting the request manager slots to the signals sent by the reply object
        QObject::connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(slotDownloadProgress(qint64, qint64)));
        QObject::connect(reply, SIGNAL(finished()), this, SLOT(slotFinished()));
        QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));

        QEventLoop *waiter = new QEventLoop(nullptr);/* Create the QEventLoop */
        d->replyWaiterMap[reply] = waiter;
        d->replyPathMap[reply] = path;
        bRes = waiter->exec() == 0;/* Execute the QEventLoop - it will quit when the above finished due to the connect() */

        if (bRes)
        {
            auto payload = reply->readAll();
            QFile outputFile(path);
            outputFile.open(QFile::WriteOnly | QFile::Truncate);
            bRes = outputFile.write(payload) == payload.size();
        }
        else
        {
            int httpCode = static_cast<int>(reply->error());
        }
    }

    return bRes;
}



//--------------------------------------------------------------------------------------------------------------------------

void medFirstStartDownloader::slotError(QNetworkReply::NetworkError err)
{
    QNetworkReply *reply = dynamic_cast<QNetworkReply*>(QObject::sender());    // retrieving the reply object who sent an error signal
    if (reply)    //handling the reply error information
    {
        d->replyWaiterMap[reply]->exit(-1);
    }
}

void medFirstStartDownloader::slotDownloadProgress(qint64 bytesSent, qint64 bytesTotal)
{    
    QNetworkReply *reply = dynamic_cast<QNetworkReply*>(QObject::sender());  // retrieving the reply object who sent a download signal
    //TODO Log
}

void medFirstStartDownloader::slotFinished()
{    
    QNetworkReply *reply = dynamic_cast<QNetworkReply*>(QObject::sender()); // retrieving the reply object who sent a finish signal
    if (reply) //handling the reply finish information
    {
        d->replyWaiterMap[reply]->exit(0);
    }
}

