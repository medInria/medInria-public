/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore>

#include "medCoreExport.h"

/**
 * @class medJobItem
 * @author Michael Knopke
 * @brief Abstract base class for all QRunnables that should appear in the Job Toolbox.
 *
 * The base class should use the signals defined here, where sender is the pointer to the object.
 * The pointer is used to track the item in the ProgressionStack, it is necessary due to the non-blocking connection of Signals and Slots between threads.
 * A non-blocking connection is needed for QThreadPool::waitForDone()
 *
 * An example of use is the following:
 *  @code
 *   medRunnableProcess *runProcess = new medRunnableProcess;
 *   runProcess->setProcess (process);
 *
 *   d->progression_stack->addJobItem(runProcess, "Progress:");
 *
 *   connect (runProcess, SIGNAL (success  (QObject*)),  this, SIGNAL (success ()));
 *   connect (runProcess, SIGNAL (failure  (QObject*)),  this, SIGNAL (failure ()));
 *   connect (runProcess, SIGNAL (cancelled (QObject*)), this, SIGNAL (failure ()));
 *
 *   medJobManager::instance()->registerJobItem(runProcess);
 *   QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(runProcess));
 *   @endcode
 *
 *  @warning : Using slots in a QRunnable is very risky. This is because
 *    a) the events may be processed in a different thread
 *    and
 *    b) the deletion is performed by a delete call in QThreadPool ( and not deleteLater() )
 *   This means that events pending for the object when the job finishes may cause the app to crash : see
 *   the warning in the doc for QObject::~QObject().
 * @sa  QObject::~QObject()
 */
class MEDCORE_EXPORT medJobItem :  public QObject, public QRunnable
{
    Q_OBJECT

public:
             medJobItem();
    virtual ~medJobItem();

signals:
    void progress  (QObject* sender, int progress);
    void progressed(int progress); // backward compatible version (automatically called when progress is emitted)
    void success   (QObject* sender);
    void failure   (QObject* sender);
    void cancelled (QObject* sender);
    void showError (const QString& message, unsigned int timeout);
    void activate(QObject* sender,bool active);

    /**
    * This signal is emitted when the process cannot be cancelled anymore.
    * @params QObject *sender
    * @return void
    */
    void disableCancel(QObject* sender);

public slots:

    /**
    * onCancel - Re-implement this, if your job is able to cancel itself (recommended).
    * It should then emit cancelled(sender) to give a status to the ProgressionStack
    * @params: QObject *sender
    * @return   void
    */
    virtual void onCancel(QObject*);

protected slots:
    void onProgress(QObject* sender, int prog);

};


