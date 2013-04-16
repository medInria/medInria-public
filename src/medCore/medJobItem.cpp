/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medJobItem.h"

medJobItem::medJobItem() : QRunnable()
{
    // Use  Qt::DirectConnection to avoid a crash when processing queued events after this has been deleted.
    connect(this, SIGNAL(progress(QObject*, int)), this, SLOT(onProgress(QObject*, int)), Qt::DirectConnection);
}

medJobItem::~medJobItem()
{

}

void medJobItem::onCancel( QObject* )
{

}

void medJobItem::onProgress( QObject* sender, int prog )
{
    Q_UNUSED(sender);
    emit progressed(prog);
}

