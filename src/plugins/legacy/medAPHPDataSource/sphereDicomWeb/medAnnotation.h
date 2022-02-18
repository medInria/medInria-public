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

class annotationDownloader;

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
    bool getAnnotationData(int pi_requestId, const QString &uid) override;
    bool isCahedDataPath(int requestId);
    QString addData(QVariant dataset, QString name, QString &seriesUid) override;

    void abortDownload(int pi_requestId) override;

public slots:
    void updateUrl(QString const &url) override;
signals:
    void operate(QString requestUrl, int requestId);

private:
    QMap<int, annotationDownloader*> requestIdMap;
    QThread workerThread;
    QString m_url;

};


