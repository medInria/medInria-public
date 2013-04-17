/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medAttachedData.h"

class medAttachedDataPrivate
{
public:
    medAbstractData* parentData; // Weak pointer, the parent has a strong pointer.
};

medAttachedData::medAttachedData( medAttachedData * parentData )
    : medAbstractData(parentData)
    , d(new medAttachedDataPrivate)
{
    d->parentData = parentData;
}

medAttachedData::~medAttachedData( void )
{
    delete d;
}

medAbstractData * medAttachedData::parentData() const
{
    return d->parentData;
}

void medAttachedData::setParentData( medAbstractData * mdata )
{
    d->parentData = mdata;
}