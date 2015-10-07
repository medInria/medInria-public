/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAttachedData.h>

class medAttachedDataPrivate
{
public:
    medAbstractData* parentData; // Weak pointer, the parent has a strong pointer.
};

//! @param parent : Set the parent which will own this dataset.
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

/**
 * The data may be reference conted by a parent dataset.
 * This is not the parent in the Qt sense (it does not take ownership). The parentData reference counts this.
 */
medAbstractData * medAttachedData::parentData() const
{
    return d->parentData;
}

void medAttachedData::setParentData( medAbstractData * mdata )
{
    d->parentData = mdata;
}
