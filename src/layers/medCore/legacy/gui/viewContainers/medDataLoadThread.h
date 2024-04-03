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

#include <medDataIndex.h>
#include <medViewContainer.h>

class medAbstractData;

class medDataLoadThread : public QObject //QThread
{
    Q_OBJECT
public:
    medDataLoadThread(medDataIndex const & index, medViewContainer *parent);
//    medDataLoadThread(medDataIndex const & index);
    virtual ~medDataLoadThread() override;

public slots:
    void process();

signals:
    void finished();
    void dataReady(medAbstractData *);

private:
    void internalProcess(int deep);

private:
    medDataIndex m_index;
    medViewContainer *m_parent;
    QList<medAbstractData *> m_pAbsDataList;
};
