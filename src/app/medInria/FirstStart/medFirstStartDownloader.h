#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QObject>
#include <QString>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>

class medFirstStartDownloader : public QObject
{
    Q_OBJECT
public:
    medFirstStartDownloader();
    ~medFirstStartDownloader();

    void askNewVersion();

    void setDownload(QMap<QString, QPair<QString, QUrl>> downloadMap);
    QMap<QString, QPair<QString, QUrl>> getDownloadedMap();
    QStringList getDownloadFailed();

private slots:
    //void toto();
    void slotDownloadProgress(qint64 bytesSent, qint64 bytesTotal);
    void currentResponse(QNetworkReply *reply, int httpVerb, int status, qint64 bytesSent, qint64 bytesTotal);
    void slotFinished();
    void slotError(QNetworkReply::NetworkError err);

private:
    QMap<QString, QPair<QString, QUrl>> m_downloadMap;
    QMap<QString, QPair<QString, QUrl>> m_downloadedMap;
    QStringList m_downloadFailed;


};
