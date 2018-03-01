/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medJobItemL.h>

#include <medCoreLegacyExport.h>

class medRunnableProcessPrivate;
class dtkAbstractProcess;

/**
 * @class medRunnableProcess
 * @author Pierre Fillard
 * @brief Wraps a dtkAbstractProcess into a medJobItemL object
 * This class allows to run a dtkAbstractProcess in a separate thread.
 * Since it derives from medJobItemL, this class benefits from the same
 * signals. It can also be used in cunjunction with the medJobManagerL.
 * As for any medJobItemL, a non-blocking connection is needed for
 * QThreadPool::waitForDone()
 */
class MEDCORELEGACY_EXPORT medRunnableProcess : public medJobItemL
{
    Q_OBJECT

public:
     medRunnableProcess();
    ~medRunnableProcess();


    void setProcess (dtkAbstractProcess *proc);
    dtkAbstractProcess * getProcess();

public slots:
    virtual void onSuccess();
    virtual void onFailure();
    virtual void onProgressed (int);

    virtual void onCancel (QObject*);

protected:
    virtual void internalRun();

private:
    medRunnableProcessPrivate *d;
};


