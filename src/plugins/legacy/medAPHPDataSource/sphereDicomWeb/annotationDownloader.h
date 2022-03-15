/*=========================================================================

 medInria

Copyright (c) INRIA 2013 - 2019. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include "medAbstractAnnotation.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>

class annotationDownloader : public QObject
{
Q_OBJECT

public:
    explicit annotationDownloader():m_requestId(-1){};
    ~annotationDownloader();

public slots:
    void doWork(const QString& requestUrl, int pi_requestId);
    void onErrorOccured(QNetworkReply::NetworkError error);
    void finish(QNetworkReply *reply);
    void inProgress(qint64 bytesReceived, qint64 bytesTotal);
    void abort();
signals:
    void downloadProgress(int requestId, medAbstractSource::eRequestStatus status);
    void pathToData(int requestId, const QString &path);

public:
    QString m_path;
    int m_requestId;

private:
    QNetworkAccessManager *m_Manager;
    QNetworkReply *m_Reply;
    QTemporaryDir m_TemporaryDir;
};
