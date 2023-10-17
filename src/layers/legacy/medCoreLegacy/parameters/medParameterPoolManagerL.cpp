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
    QHash<QString, std::shared_ptr<medParameterPoolL> > pools;
};

std::unique_ptr<medParameterPoolManagerL> medParameterPoolManagerL::s_instance = nullptr;

medParameterPoolManagerL &medParameterPoolManagerL::instance()
{
    if(!s_instance)
    {
        s_instance = std::unique_ptr<medParameterPoolManagerL>(new medParameterPoolManagerL());
    }
    return *s_instance.get();
}

medParameterPoolManagerL::medParameterPoolManagerL(void) : d(new medParameterPoolManagerLPrivate)
{
}

medParameterPoolManagerL::~medParameterPoolManagerL() = default; // For private class forward declaration

void medParameterPoolManagerL::removePool(QString poolId)
{
    auto poolToRemove = d->pools.value(poolId);
    if(poolToRemove)
    {
        poolToRemove->clear();
        d->pools.remove(poolId);
    }
}

void medParameterPoolManagerL::linkParameter(medAbstractParameterL* parameter , QString poolId)
{
    if( !d->pools.keys().contains(poolId) )
    {
        std::shared_ptr<medParameterPoolL> selectedPool(new medParameterPoolL(this));
        selectedPool->setName(poolId);
        d->pools.insert(poolId, selectedPool);
        selectedPool->append(parameter);
    }
    else
    {
        d->pools.value(poolId)->append(parameter);
    }
}

void medParameterPoolManagerL::unlinkParameter(medAbstractParameterL* param)
{
    for(const auto& pool : d->pools.values() )
    {
        pool->remove(param);
    }
}

QList<std::shared_ptr<medParameterPoolL> > medParameterPoolManagerL::pools()
{
    return d->pools.values();
}

QStringList medParameterPoolManagerL::pools(medAbstractParameterL *param)
{
    QStringList pools;
    for(const auto& pool : d->pools.values() )
    {
        if(pool->parameters().contains(param))
        {
            pools << pool->name();
        }
    }
    return pools;
}

std::shared_ptr<medParameterPoolL> medParameterPoolManagerL::pool(QString poolId)
{
    return d->pools.value(poolId);
}
