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

public slots:
    void doWork(const QString& requestUrl, int pi_requestId);
    void onErrorOccured(QNetworkReply::NetworkError error);
    void finish(QNetworkReply *reply);
    void inProgress(qint64 bytesReceived, qint64 bytesTotal);

signals:
    void downloadProgress(int requestId, medAbstractSource::eRequestStatus status);
    void pathToData(int requestId, const QString &path);

private:
    QNetworkAccessManager *m_Manager;
    QNetworkReply *m_Reply;
    QTemporaryDir m_TemporaryDir;
    int m_requestId;
};


class medAnnotation : public medAbstractAnnotation
{
    Q_OBJECT
public:
    medAnnotation();
    ~medAnnotation();


    /* ***********************************************************************/
    /* **************************** Check Connection *************************/
    /* ***********************************************************************/
    int isServerAvailable() override;


    /* ***********************************************************************/
    /* **************************** Request Get  *****************************/
    /* ***********************************************************************/
    QList<QMap<QString, QString>> findAnnotationMinimalEntries(const QString &seriesInstanceUID) override;
    int getAnnotationData(const QString &uid) override;
    QString addData(QVariant dataset, QString name, QString &seriesUid) override;

signals:
    void operate(QString requestUrl, int requestId);

private:
    QMap<int, annotationDownloader*> requestIdMap;
    QThread workerThread;
    static int requestId;
};


