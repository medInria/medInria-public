#include "medAnnotationData.h"

#include <medCore/medAbstractData.h>

#include <dtkCore/dtkSmartPointer.h>

class medAnnotationData::medAnnotationDataPrivate
{
public:
    dtkSmartPointer<medAbstractData> parentData;
};

medAnnotationData::medAnnotationData()
    : medAttachedData(),
    d( new medAnnotationDataPrivate )
{
}

medAnnotationData::~medAnnotationData()
{
    delete d;
    d = NULL;
}

medAbstractData * medAnnotationData::parentData() const
{
    return d->parentData;
}

void medAnnotationData::setParentData( medAbstractData * mdata )
{
    d->parentData = mdata;
}



