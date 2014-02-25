/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
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

/**
* Specify which dtk process to run.
*/
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
        if (d->process->update() == 0)
            emit success (this);
        else
            emit failure (this);
    }

    // See above comment. Don't remove.
    deleteLater();
}

/**
* dtkAbstractProcess signals success(), failure() and progressed(int)
* need to be translated into corresponding medJobItem signals taking
* in argument the pointer of the object. This is the role of those
* slots.
*/
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

/**
* Contrarily to success() and failure(), the cancel() method is called
* from outside this object (success and failure and emitted by the process
* itself. This slot implements the expected behaviour when a cancel request
* was made by calling the approrite onCanceled() slot of the running
* dtkAbstractProcess
*/
void medRunnableProcess::onCancel (QObject *sender)
{
    if (d->process) {
        //d->process->onCanceled();
        d->process->cancel();
    }

    emit (cancelled (this));
}
