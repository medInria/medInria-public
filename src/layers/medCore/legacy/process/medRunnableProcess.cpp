/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medAbstractProcessLegacy.h"

#include <medRunnableProcess.h>

#include <dtkCoreSupport/dtkAbstractProcess.h>
#include <dtkCoreSupport/dtkSmartPointer.h>

class medRunnableProcessPrivate
{
public:
    dtkSmartPointer<dtkAbstractProcess> process;
};


medRunnableProcess::medRunnableProcess(void): medJobItemL(), d (new medRunnableProcessPrivate)
{
}

medRunnableProcess::~medRunnableProcess()
{
    delete d;
    d = nullptr;
}

/**
* Specify which dtk process to run.
*/
void medRunnableProcess::setProcess (dtkAbstractProcess *proc)
{
    d->process = proc;

    if (d->process)
    {
        connect (d->process, SIGNAL (progressed(int)), this, SLOT (onProgressed(int)), Qt::UniqueConnection);
    }
}

dtkAbstractProcess * medRunnableProcess::getProcess()
{
    return d->process;
}

void medRunnableProcess::internalRun()
{
    if (d->process)
    {
        int res = d->process->update();

        if (res == medAbstractProcessLegacy::SUCCESS)
        {
            emit success (this);
        }
        else
        {
            emit failure (this);
            emit failure (res);
        }
    }
}

/**
* dtkAbstractProcess signals success(), failure() and progressed(int)
* need to be translated into corresponding medJobItemL signals taking
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
* Contrary to success() and failure(), the cancel() method is called
* from outside this object (success and failure and emitted by the process
* itself). This slot implements the expected behaviour when a cancel request
* was made by calling the appropriate onCanceled() slot of the running
* dtkAbstractProcess
*/
void medRunnableProcess::onCancel (QObject *sender)
{
    Q_UNUSED(sender)

    if (d->process)
    {
        d->process->cancel();
    }

    emit (cancelled (this));
}
