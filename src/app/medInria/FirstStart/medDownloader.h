#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/



#include <QBytearray>
#include <QNetworkaccessmanager>
#include <QNetworkreply>
#include <QObject>
#include <QString>
#include <QUrl>

class medDownloader : public QObject
{
    Q_OBJECT
public:
    medDownloader();
    ~medDownloader();
    
    int download(QUrl url);
    QString getName();
    QString getMime();
    QString getExt();

private slots:

    void slotDownloadProgress(qint64 bytesSent, qint64 bytesTotal);
    void currentResponse(QNetworkReply *reply, int httpVerb, int status, qint64 bytesSent, qint64 bytesTotal);
    void slotFinished();
    void extractSinglePart(QByteArray &payload);
    void slotError(QNetworkReply::NetworkError err);

private:


private:
    QNetworkAccessManager * m_qnam;
};

 void extractMultiPart(QByteArray &boundary, QByteArray &payload);
