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
#include <QNetworkReply>

class QNetworkAccessManager;

class medFirstStartDownloaderPrivate;
class medFirstStartDownloader : public QObject
{
    Q_OBJECT
public:
    medFirstStartDownloader(QNetworkAccessManager * qnam);
    ~medFirstStartDownloader();

    bool download(QString url, QString path);

private slots:
    void slotError(QNetworkReply::NetworkError err);
    void slotDownloadProgress(qint64 bytesSent, qint64 bytesTotal);
    void slotFinished();

private:
    medFirstStartDownloaderPrivate *d;
};
