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

/**
 * @class medFirstStartDownloader
 * @brief Downloads files during the first application start.
 *
 * This class handles the download of files necessary for the first application
 * start. It uses a `QNetworkAccessManager` to perform the download and emits
 * signals to notify the user about the progress and errors.
 */
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
