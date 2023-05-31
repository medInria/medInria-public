/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medParameterPoolManagerL.h"

#include <medAbstractView.h>
#include <medAbstractParameterL.h>
#include <medParameterPoolL.h>

class medParameterPoolManagerLPrivate
{
public:
    QHash<QString, medParameterPoolL *> pools;

};

std::shared_ptr<medParameterPoolManagerL> medParameterPoolManagerL::s_instance = nullptr;

medParameterPoolManagerL *medParameterPoolManagerL::instance()
{
    if(!s_instance)
    {
        s_instance = std::shared_ptr<medParameterPoolManagerL>(new medParameterPoolManagerL());
    }
    return s_instance.get();
}

medParameterPoolManagerL::medParameterPoolManagerL(void) : d(new medParameterPoolManagerLPrivate)
{
}

medParameterPoolManagerL::~medParameterPoolManagerL()
{
    for (auto it = d->pools.begin(); it != d->pools.end(); ++it)
    {
        delete it.value();
    }
    d->pools.clear();

    delete d;
    d = nullptr;

    s_instance.reset();
}

void medParameterPoolManagerL::removePool(QString poolId)
{
    medParameterPoolL *poolToRemove = d->pools.value(poolId);
    if(poolToRemove)
    {
        poolToRemove->clear();
        d->pools.remove(poolId);
        delete poolToRemove;
    }
}

void medParameterPoolManagerL::linkParameter(medAbstractParameterL* parameter , QString poolId)
{
    medParameterPoolL *selectedPool;

    if( !d->pools.keys().contains(poolId) )
    {
        selectedPool = new medParameterPoolL(this);
        selectedPool->setName(poolId);
        d->pools.insert(poolId, selectedPool);
    }
    else selectedPool = d->pools.value(poolId);

    selectedPool->append(parameter);
}

void medParameterPoolManagerL::unlinkParameter(medAbstractParameterL* param)
{
    for(medParameterPoolL *pool : d->pools.values() )
    {
        pool->remove(param);
    }
}

QList<medParameterPoolL*> medParameterPoolManagerL::pools()
{
    return d->pools.values();
}

QStringList medParameterPoolManagerL::pools(medAbstractParameterL *param)
{
    QStringList pools;
    for(medParameterPoolL *pool: d->pools.values() )
    {
        if(pool->parameters().contains(param))
        {
            pools << pool->name();
        }
    }
    return pools;
}

medParameterPoolL* medParameterPoolManagerL::pool(QString poolId)
{
    return d->pools.value(poolId);
}
