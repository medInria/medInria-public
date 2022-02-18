/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "annotationDownloader.h"

#include <QEventLoop>
#include <QNetworkReply>
#include <QJsonDocument>

annotationDownloader::~annotationDownloader()
{
    qDebug()<<"delete annottaionDownloader";
}

void annotationDownloader::doWork(const QString& requestUrl, int pi_requestId)
{
    m_requestId = pi_requestId;
    m_Manager = new QNetworkAccessManager(this);
    m_Reply = m_Manager->get(QNetworkRequest(QUrl(requestUrl)));

    QObject::connect(m_Manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(finish(QNetworkReply *)));
    QObject::connect(m_Reply, SIGNAL(errorOccurred(QNetworkReply::NetworkError)), this, SLOT(onErrorOccured(
            QNetworkReply::NetworkError)));
    QObject::connect(m_Reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(inProgress(qint64, qint64)));

}

void annotationDownloader::onErrorOccured(QNetworkReply::NetworkError error)
{
    if (error != QNetworkReply::NoError )
    {
        // TODO To improve, check the value of error and decide what to do !
        emit downloadProgress(m_requestId, medAbstractSource::eRequestStatus::cnxLost);
    }
}

void annotationDownloader::inProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    emit downloadProgress(m_requestId, medAbstractSource::eRequestStatus::pending);
}

void annotationDownloader::finish(QNetworkReply *reply)
{
    switch (reply->error())
    {
        case QNetworkReply::NoError:
        {
            emit downloadProgress(m_requestId, medAbstractSource::eRequestStatus::finish);
            QByteArray data = reply->readAll();
            if (m_TemporaryDir.isValid())
            {
                QDir dir(m_TemporaryDir.path());
                dir.mkdir(  QString::number(m_requestId));
                m_path = dir.filePath(QString::number(m_requestId) + "/tmpAnnotation.mha");
                QFile file(m_path);
                file.open(QIODevice::WriteOnly);
                file.write(data);
                file.close();
                emit pathToData(m_requestId, m_path);
            }
            break;
        }

        case QNetworkReply::ConnectionRefusedError:
        default:
            emit downloadProgress(m_requestId, medAbstractSource::eRequestStatus::faild);
            break;
    }

}

void annotationDownloader::abort()
{
    if (m_Reply)
    {
        m_Reply->abort();
    }
}



