/* medRunnableProcess.h --- 
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

#pragma once

#include "medJobItem.h"

#include "medCoreExport.h"

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
     medRunnableProcess(void);
    ~medRunnableProcess(void);

    void run(void);

    /**
    * Specify which dtk process to run.
    */
    void setProcess (dtkAbstractProcess *proc);

public slots:
    /**
    * dtkAbstractProcess signals success(), failure() and progressed(int)
    * need to be translated into corresponding medJobItem signals taking
    * in argument the pointer of the object. This is the role of those
    * slots.
    */
    virtual void onSuccess    (void);
    virtual void onFailure    (void);
    virtual void onProgressed (int);

    /**
    * Contrarily to success() and failure(), the cancel() method is called
    * from outside this object (success and failure and emitted by the process
    * itself. This slot implements the expected behaviour when a cancel request
    * was made by calling the approrite onCanceled() slot of the running
    * dtkAbstractProcess
    */
    virtual void onCancel (QObject*);
    
private:
    medRunnableProcessPrivate *d;
};


