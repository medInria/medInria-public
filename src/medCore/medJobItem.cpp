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

