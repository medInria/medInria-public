#include "medAbstractData.h"

#include "medAttachedData.h"

class medAbstractDataPrivate
{
public:
    bool isTrueVolumetric;
    QList< medAttachedData * > attachedData;
};

medAbstractData::medAbstractData( dtkAbstractData *parent )
    : dtkAbstractData(parent)
    , d(new medAbstractDataPrivate)
{
    d->isTrueVolumetric = false;
}


medAbstractData::~medAbstractData( void )
{
    delete d;
    d = NULL;
}

void medAbstractData::setTrueVolumetric( bool flag )
{
    d->isTrueVolumetric = flag;
}

bool medAbstractData::trueVolumetric()
{
    return d->isTrueVolumetric;
}

QList< medAttachedData * > medAbstractData::attachedData() const
{
    return d->attachedData;
}

void medAbstractData::clearAttachedData()
{
    foreach ( medAttachedData * d, d->attachedData )
        emit attachedDataRemoved( d );
    d->attachedData.clear();
}

void medAbstractData::addAttachedData( medAttachedData * data )
{
    if ( !d->attachedData.contains( data ) ) {
        d->attachedData.append( data );
        emit attachedDataAdded( data );
    }
}

void medAbstractData::removeAttachedData( medAttachedData * data )
{
    int n = d->attachedData.count();
    d->attachedData.removeAll( data );
    if ( n > d->attachedData.count() )
        emit attachedDataRemoved( data );
}

