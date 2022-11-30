/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medAbstractAnnotation.h"
#include "annotationRequest.h"

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

    bool hasUrl() override;

    /* ***********************************************************************/
    /* **************************** Request Get  *****************************/
    /* ***********************************************************************/
    QList<QMap<QString, QString>> findAnnotationMinimalEntries(const QString &seriesInstanceUID) override;
    bool getAnnotationData(int pi_requestId, const QString &uid) override;
    bool isCachedDataPath(int requestId);
    bool addData(int requestId, QVariant dataset, QString name, QString &seriesUid) override;

    void abortDownload(int pi_requestId) override;

public slots:
    void updateUrl(QString const &url) override;

private:
    QMap<int, AnnotationRequest*> requestIdMap;
    QNetworkAccessManager m_Manager;
    QString m_url;

};


