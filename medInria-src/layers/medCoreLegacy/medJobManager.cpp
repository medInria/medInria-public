/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medJobManager.h>
#include <medJobItem.h>

medJobManager *medJobManager::s_instance = NULL;

class medJobManagerPrivate
{
public:
    QList<medJobItem*> itemList;
    bool m_IsActive;
};

medJobManager *medJobManager::instance(void)
{
    if(!s_instance)
        s_instance = new medJobManager;

    return s_instance;
}

medJobManager::medJobManager( void ) : d(new medJobManagerPrivate)
{
    d->m_IsActive = true;
}

medJobManager::~medJobManager( void )
{
    delete d;

    d = NULL;
}

bool medJobManager::registerJobItem( medJobItem* item, QString jobName)
{
    if(d->m_IsActive)
    {
        d->itemList.append(item);
        connect(this, SIGNAL(cancel(QObject*)), item, SLOT(onCancel(QObject*)) );
        emit jobRegistered(item, jobName);
        return true;
    }
    return false;
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

void medJobManager::dispatchGlobalCancelEvent(bool ignoreNewJobItems)
{
    if (ignoreNewJobItems)
        d->m_IsActive = false;
    foreach( medJobItem* item, d->itemList )
        emit cancel( item );
}
