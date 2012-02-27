/* medRunnableProcess.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:40:14 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:40:15 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

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

medRunnableProcess::~medRunnableProcess (void)
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

void medRunnableProcess::run (void)
{
    if (d->process) {
        if (d->process->update()==0)
            emit success (this);
        else
            emit failure (this);
    }
}

void medRunnableProcess::onSuccess (void)
{
    emit success (this);
}

void medRunnableProcess::onFailure (void)
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
