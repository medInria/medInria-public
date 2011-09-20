#include "medJobItem.h"

medJobItem::medJobItem() : QRunnable()
{
    connect(this, SIGNAL(progressed(QObject*, int)), this, SLOT(onProgressed(QObject*, int)));
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

