/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medJobManager.h>
#include <medAbstractJob.h>

medJobManager *medJobManager::s_instance = NULL;

class medJobManagerPrivate
{
public:
    QList<medAbstractJob*> itemList;
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

/**
* registerJob - register a job item if you want that the manager sends cancel events to them (highly suggested!)
* The manager will reject items if not active (see dispatchGlobalCancelEvent)
* @param: const medAbstractJob & item
* @param: QString jobName short name that will be visible on the progression toolboxes
* @return   bool
*/
bool medJobManager::registerJob(medAbstractJob* job, QString jobName)
{
    if(d->m_IsActive)
    {
        d->itemList.append(job);
        connect(this, SIGNAL(cancel(QObject*)), job, SLOT(onCancel(QObject*)) );
        emit jobRegistered(job, jobName);
        return true;
    }
    return false;
}
/**
* unRegisterJob - remove the job from the hash
* @param: const medAbstractJob & item
* @return   bool
*/
bool medJobManager::unRegisterJob(medAbstractJob* job )
{
    int index = d->itemList.indexOf(job);
    if (index != -1)
    {
        disconnect(this, SIGNAL(cancel(QObject*)), job, SLOT(onCancel(QObject*)) );
        d->itemList.removeAt(index);
        return true;
    }
    return false;
}

/**
* dispatchGlobalCancelEvent - emits a cancel request to all registered items
* @param bool ignoreNewJobItems - if set (default) the manager will not register any new items
* @return   void
*/
void medJobManager::dispatchGlobalCancelEvent(bool ignoreNewJobItems)
{
    if (ignoreNewJobItems)
        d->m_IsActive = false;
    foreach( medAbstractJob* item, d->itemList )
        emit cancel( item );
}
