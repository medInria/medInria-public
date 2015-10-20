/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medJobManager.h>

#include <QApplication>
#include <QThreadPool>

#include <dtkLog>

#include <medAbstractJob.h>

medJobManager* medJobManager::s_instance = NULL;

medJobManager* medJobManager::instance()
{
    if(s_instance == NULL)
        s_instance = new medJobManager(QApplication::instance());
        // delete is delegate to the QApplication instance.
    return s_instance;
}

class medJobManagerPrivate
{
public:
    QList<medAbstractJob *> jobs;
};

medJobManager::medJobManager(QObject *parent)
    : QObject(parent), d(new medJobManagerPrivate)
{
}

medJobManager::~medJobManager()
{
    // just in case some unparented job are still living.
    for(medAbstractJob* job : d->jobs)
    {
        dtkWarn() << "Orphan job still living at the end of the app detected:"
                  << job->caption() << job->staticMetaObject.className() << job;
        delete job;
    }
}

void medJobManager::registerJob(medAbstractJob *job)
{
    d->jobs << job;
}

void medJobManager::unregisterJob(medAbstractJob *job)
{
    d->jobs.removeAll(job);
}

QList<medAbstractJob *> medJobManager::jobs() const
{
    return d->jobs;
}

void medJobManager::cancelAll()
{
    for(medAbstractJob *job : d->jobs)
        job->cancel();
}

void medJobManager::startJobInThread(medAbstractJob *job, int priority)
{
    emit job->_running(true);
    QThreadPool::globalInstance()->start(job, priority);
}




