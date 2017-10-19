/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medJobItem.h>

medJobItem::medJobItem() : QRunnable()
{
    connect(this, SIGNAL(progress(QObject*, int)), this, SLOT(onProgress(QObject*, int)));
}

medJobItem::~medJobItem()
{

}

void medJobItem::run()
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

    internalRun();

    // See above comment. Don't remove.
    deleteLater();

}

void medJobItem::onCancel( QObject* )
{

}

void medJobItem::onProgress( QObject* sender, int prog )
{
    Q_UNUSED(sender);
    emit progressed(prog);
}

