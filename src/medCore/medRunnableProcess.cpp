/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medRunnableProcess.h"

#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkSmartPointer.h>

class medRunnableProcessPrivate
{
public:
    dtkSmartPointer<dtkAbstractProcess> process;
};


medRunnableProcess::medRunnableProcess(void): medJobItem(), d (new medRunnableProcessPrivate)
{
    // d->process = 0; // automatic.
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
        // QueuedConnection does not seem to work (never called). Changing it to BlockingQueuedConnection
        // may result in a dead lock. Then, it is better to emit those directly from the run() method.
        // connect (d->process, SIGNAL (success()),       this, SLOT (onSuccess()), Qt::QueuedConnection);
        // connect (d->process, SIGNAL (failure()),       this, SLOT (onFailure()), Qt::QueuedConnection);
	connect (d->process, SIGNAL (progressed(int)), this, SLOT (onProgressed(int)), Qt::QueuedConnection);
    }
}

dtkAbstractProcess * medRunnableProcess::getProcess()
{
    return d->process;
}

void medRunnableProcess::run()
{
    if (d->process) {
        if (d->process->update()==0)
            emit success (this);
        else
            emit failure (this);
    }
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
