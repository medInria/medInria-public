#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.

=========================================================================*/

#include <QObject>
#include <QThread>
#include <QUrl>

#include <medDataIndex.h>
#include <medViewContainer.h>

class medAbstractData;

class medDataLoadThread : public QObject
{
    Q_OBJECT
public:
    medDataLoadThread(QList<medDataIndex>  const & index, QList<QUrl> const & urls, medViewContainer *parent);
    virtual ~medDataLoadThread() override = default ;

public slots:
    void process();

signals:
    void finished();
    void dataReady(medAbstractData *);

private:
    void internalProcess(medDataIndex &index, int deep);

private:
    QList<QUrl> m_urlList;
    QList<medDataIndex> m_indexList;
    medViewContainer *m_parent;
    QList<medAbstractData *> m_pAbsDataList;
    QMap<QString, QString> m_volumePathsMap;
    QMap<QString, QPair<QString, QString>> m_volumeRelMap;
    QString m_rootDir;
};
