#include "medAbstractData.h"

#include "medDataIndex.h"

class medAbstractDataPrivate
{
public:
    bool isTrueVolumetric;
    medDataIndex index;
};

medAbstractData::medAbstractData( dtkAbstractData *parent /*= 0*/ ) : dtkAbstractData(parent), d(new medAbstractDataPrivate)
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
