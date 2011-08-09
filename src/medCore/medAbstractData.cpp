#include "medAbstractData.h"

#include "medDataIndex.h"
#include "medAttachedData.h"

class medAbstractDataPrivate
{
public:
    bool isTrueVolumetric;
    medDataIndex index;
    QList< medAttachedData * > attachedData;
};

medAbstractData::medAbstractData( dtkAbstractData *parent )
    : dtkAbstractData(parent)
    , d(new medAbstractDataPrivate)
{
    d->isTrueVolumetric = false;
    qDebug() << "constructing medAbstractData: ";
}


medAbstractData::~medAbstractData( void )
{
    qDebug() << "deleting data with index " << d->index.asString();
    delete d;
    d = NULL;
}

void medAbstractData::setTrueVolumetric( bool flag )
{
    d->isTrueVolumetric = flag;
}

bool medAbstractData::trueVolumetric() const
{
    return d->isTrueVolumetric;
}

void medAbstractData::setDataIndex( const medDataIndex& index )
{
    // copy ids
    d->index.setDataSourceId(index.dataSourceId());
    d->index.setPatientId(index.patientId());
    d->index.setStudyId(index.studyId());
    d->index.setSeriesId(index.seriesId());
    d->index.setImageId(index.imageId());
}

medDataIndex medAbstractData::dataIndex() const
{
    return d->index;
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
