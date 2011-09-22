#include "medJobItem.h"

medJobItem::medJobItem() : QRunnable()
{
    // Use  Qt::DirectConnection to avoid a crash when processing queued events after this has been deleted.
    connect(this, SIGNAL(progressed(QObject*, int)), this, SLOT(onProgressed(QObject*, int)), Qt::DirectConnection);
}

medJobItem::~medJobItem()
{

}

void medJobItem::onCancel( QObject* )
{
     
}

void medJobItem::onProgressed( QObject* sender, int prog )
{
    emit progress(prog);
}

