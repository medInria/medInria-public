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
    QHash<QString, medParameterPoolL> pools;
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
    if(d->pools.keys().contains(poolId))
    {
        medParameterPoolL &poolRef = d->pools[poolId];
        poolRef.clear();
        d->pools.remove(poolId);
    }
}

void medParameterPoolManagerL::linkParameter(medAbstractParameterL* parameter , QString poolId)
{
    if( !d->pools.keys().contains(poolId) )
    {
        d->pools[poolId].setName(poolId);
        d->pools[poolId].setParent(this);
    }

    d->pools[poolId].append(parameter);
}

void medParameterPoolManagerL::unlinkParameter(medAbstractParameterL* param)
{
    for(auto &key : d->pools.keys() )
    {
        d->pools[key].remove(param);
    }
}

QList<medParameterPoolL *> medParameterPoolManagerL::pools()
{
    QList<medParameterPoolL *> poolsPointers;
    
    for(auto &key : d->pools.keys() )
    {
        poolsPointers.push_back(&d->pools[key]);
    }
    return poolsPointers;
}

QStringList medParameterPoolManagerL::pools(medAbstractParameterL *param)
{
    QStringList pools;
    
    for(auto &key : d->pools.keys() )
    {
        if(d->pools[key].parameters().contains(param))
        {
            pools << d->pools[key].name();
        }
    }
    
    return pools;
}

medParameterPoolL* medParameterPoolManagerL::pool(QString poolId)
{
    medParameterPoolL *poolPointer = nullptr;

    if(d->pools.keys().contains(poolId))
    {
        poolPointer = &d->pools[poolId];
    }

    return poolPointer;
}
