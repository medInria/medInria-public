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
    // register medAbstractJob::medJobExitStatus at run-time
    // to use the type it in queued signal and slot connections
    qRegisterMetaType<medAbstractJob::medJobExitStatus>("medJobExitStatus");
}

medJobManager::~medJobManager()
{
    // just in case some unparented job are still living.
    for(medAbstractJob* job : d->jobs)
    {
        dtkDebug() << "Orphan job still living at the end of the app detected:"
                 << job->caption() << job;
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

void medJobManager::startJobInThread(medAbstractJob *job)
{
    QThreadPool::globalInstance()->start(new medJobRunner(job));
}

medJobRunner::medJobRunner(medAbstractJob *job)
    : QObject(NULL)
{
    m_job = job;
}

void medJobRunner::run()
{
    emit m_job->running(true);
    medAbstractJob::medJobExitStatus jobExitStatus = medAbstractJob::MED_JOB_EXIT_FAILURE;
    try
    {
        jobExitStatus = m_job->run();
        if(jobExitStatus == medAbstractJob::MED_JOB_EXIT_CANCELLED)
        {
            dtkDebug() << "job aborted (cancelled)"
                       << m_job->caption() << m_job;
        }
    }
    catch(std::exception &err)
    {
        QString errorMessage = QString::fromLatin1(err.what());
        dtkWarn() << "Error occured while runing job"
                  << m_job->caption() << m_job
                  << "\n\t" <<errorMessage;

        emit exceptionCaught(errorMessage);
    }
    catch(...)
    {
        dtkWarn() << "Error occured while runing job"
                  << m_job->caption() << m_job;
    }
    emit m_job->finished(jobExitStatus);
    emit m_job->running(false);
}
