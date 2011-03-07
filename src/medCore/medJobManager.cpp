#include "medJobManager.h"
#include "medJobItem.h"

medJobManager *medJobManager::s_instance = NULL;

class medJobManagerPrivate
{
public:
    QList<medJobItem*> itemList;
};

medJobManager *medJobManager::instance(void)
{
    if(!s_instance)
        s_instance = new medJobManager;

    return s_instance;
}

medJobManager::medJobManager( void ) : d(new medJobManagerPrivate)
{
}

medJobManager::~medJobManager( void )
{
    delete d;

    d = NULL;
}

bool medJobManager::registerJobItem( medJobItem* item )
{
    d->itemList.append(item);
    connect(this, SIGNAL(cancel(QObject*)), item, SLOT(onCancel(QObject*)) );
    return true;
}

bool medJobManager::unRegisterJobItem( medJobItem* item )
{
    int index = d->itemList.indexOf(item);
    if (index != -1)
    {
        disconnect(this, SIGNAL(cancel(QObject*)), item, SLOT(onCancel(QObject*)) );
        d->itemList.removeAt(index);
        return true;
    }
    return false;
}

void medJobManager::dispatchGlobalCancelEvent()
{
    foreach( medJobItem* item, d->itemList )
        emit cancel( item );
}
