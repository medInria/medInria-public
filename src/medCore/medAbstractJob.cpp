/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractJob.h>
#include <medJobManager.h>

#include <medMessageController.h>


medAbstractJob::medAbstractJob(QString name) : QRunnable()
{
    medJobManager::instance()->registerJob(this, name);

    medMessageProgress *message = medMessageController::instance()->showProgress(name);

    connect(this, SIGNAL(progressed(int)),    message, SLOT(setProgress(int)));
    connect(this, SIGNAL(success()), message, SLOT(success()));
    connect(this, SIGNAL(failure()), message, SLOT(failure()));
    connect(this,SIGNAL(showError(const QString&,unsigned int)),
            medMessageController::instance(),SLOT(showError(const QString&,unsigned int)));

}

medAbstractJob::~medAbstractJob()
{
    medJobManager::instance()->unRegisterJob(this);
}

void medAbstractJob::start()
{
    QThreadPool::globalInstance()->start(this);
}

void medAbstractJob::run()
{
    /* OK, so don't touch this setAutoDelete(false) nor the deleteLater() below
    * if you don't know what you're doing, here's why :
    *
    * This run() method runs into a thread different from the main GUI thread,
    * but the thread() attribute on the medRunnableProcess is set to the main
    * GUI thread, meaning all signals sent to this medRunnableObject will trigger
    * slots to be run in the main GUI thread. Problem is, if we have the auto-delete
    * activated in the QThreadPool, this object will be deleted (not deleteLater'd)
    * and if it has pending events, it might trigger a slot on it after it was
    * deleted, and boom, crash.
    * So we disable the autoDelete and replace it with a deleteLater(), which
    * garantees us that the object will not have any pending events before
    * deletion.
    */
    setAutoDelete(false);
    m_cancel  = false;
    internalRun();
    // See above comment. Don't remove.
    deleteLater();
}

void medAbstractJob::cancel()
{
    m_cancel  = true;
}

/**
* cancel - Re-implement this, if your job is able to cancel itself (recommended).
* It should then emit cancelled(sender) to give a status to the ProgressionStack
* @return   void
*/
bool medAbstractJob::isCancelled() const
{
    return m_cancel;
}

