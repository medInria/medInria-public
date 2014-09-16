/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medJobItem.h>

#include <medCoreExport.h>

class medRunnableProcessPrivate;
class dtkAbstractProcess;

/**
 * @class medRunnableProcess
 * @author Pierre Fillard
 * @brief Wraps a dtkAbstractProcess into a medJobItem object
 * This class allows to run a dtkAbstractProcess in a separate thread.
 * Since it derives from medJobItem, this class benefits from the same
 * signals. It can also be used in cunjunction with the medJobManager.
 * As for any medJobItem, a non-blocking connection is needed for
 * QThreadPool::waitForDone()
 */
class MEDCORE_EXPORT medRunnableProcess : public medJobItem
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


