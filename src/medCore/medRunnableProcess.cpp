/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medRunnableProcess.h>

#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkSmartPointer.h>

class medRunnableProcessPrivate
{
public:
    dtkSmartPointer<dtkAbstractProcess> process;
};


medRunnableProcess::medRunnableProcess(void): medJobItem(), d (new medRunnableProcessPrivate)
{
}

medRunnableProcess::~medRunnableProcess()
{
    delete d;
    d = NULL;
}

void medRunnableProcess::setProcess (dtkAbstractProcess *proc)
{
    d->process = proc;

    if (d->process) {
        connect (d->process, SIGNAL (progressed(int)), this, SLOT (onProgressed(int)));
    }
}

dtkAbstractProcess * medRunnableProcess::getProcess()
{
    return d->process;
}

void medRunnableProcess::run()
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

    if (d->process) {
        if (d->process->update()==0)
            emit success (this);
        else
            emit failure (this);
    }

    // See above comment. Don't remove.
    deleteLater();
}

void medRunnableProcess::onSuccess()
{
    emit success (this);
}

void medRunnableProcess::onFailure()
{
    emit failure (this);
}

void medRunnableProcess::onProgressed (int value)
{
    emit activate (this, false);
    emit progress (this, value);
}

void medRunnableProcess::onCancel (QObject *sender)
{
    if (d->process) {
        //d->process->onCanceled();
        d->process->cancel();
    }

    emit (cancelled (this));
}
