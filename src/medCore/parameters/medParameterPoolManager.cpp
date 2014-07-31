/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medParameterPoolManager.h"

#include <medAbstractView.h>
#include <medAbstractParameter.h>
#include <medParameterPool.h>


class medParameterPoolManagerPrivate
{
public:
    QHash<QString, medParameterPool *> pools;

};

medParameterPoolManager *medParameterPoolManager::instance(void)
{
    if(!s_instance)
        s_instance = new medParameterPoolManager;

    return s_instance;
}

medParameterPoolManager::medParameterPoolManager(void) : d(new medParameterPoolManagerPrivate)
{
}

void medParameterPoolManager::removePool(QString poolId)
{
    medParameterPool *poolToRemove = d->pools.value(poolId);
    if(poolToRemove)
    {
        poolToRemove->clear();
        d->pools.remove(poolId);
        delete poolToRemove;
    }
}

void medParameterPoolManager::linkParameter(medAbstractParameter* parameter , QString poolId)
{
    medParameterPool *selectedPool;

    if( !d->pools.keys().contains(poolId) )
    {
        selectedPool = new medParameterPool(this);
        selectedPool->setName(poolId);
        d->pools.insert(poolId, selectedPool);
    }
    else selectedPool = d->pools.value(poolId);

    selectedPool->append(parameter);
}

void medParameterPoolManager::unlinkParameter(medAbstractParameter* param)
{
    foreach(medParameterPool *pool, d->pools.values() )
    {
        pool->remove(param);

        /*if(pool->count() == 0)
            removePool(poolId);*/
    }
}

QList<medParameterPool*> medParameterPoolManager::pools()
{
    return d->pools.values();
}

QStringList medParameterPoolManager::pools(medAbstractParameter *param)
{
    QStringList pools;
    foreach(medParameterPool *pool, d->pools.values() )
    {
        if(pool->parameters().contains(param))
            pools << pool->name();
    }
    return pools;
}

medParameterPool* medParameterPoolManager::pool(QString poolId)
{
    return d->pools.value(poolId);
}

medParameterPoolManager *medParameterPoolManager::s_instance = NULL;
