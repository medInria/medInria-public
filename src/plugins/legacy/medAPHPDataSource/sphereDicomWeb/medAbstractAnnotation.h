#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QString>
#include <QObject>
#include <medAbstractSource.h>

class medAbstractAnnotation : public QObject
{
    Q_OBJECT

public:
    virtual int isServerAvailable() = 0;
    virtual QList<QMap<QString, QString>> findAnnotationMinimalEntries(const QString &seriesInstanceUID) = 0;
    virtual bool getAnnotationData(int pi_requestId, const QString &uid) = 0;
    virtual bool isCachedDataPath(int requestId) = 0;
    virtual QString addData(QVariant variant, QString name, QString &key) = 0;
    virtual void abortDownload(int pi_requestId) = 0;

public slots:
    virtual void updateUrl(QString const &url) = 0;

signals:
    void inProgress(int requestId, medAbstractSource::eRequestStatus status);
    void pathToData(int id, const QString &path);

};