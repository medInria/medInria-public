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