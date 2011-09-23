#include "medAnnotationData.h"

#include <medCore/medAbstractData.h>

#include <dtkCore/dtkSmartPOinter.h>

class medAnnotationData::medAnnotationDataPrivate
{
public:
    dtkSmartPointer<medAbstractData> parentData;
};

medAnnotationData::medAnnotationData()
    : dtkAbstractData(),
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



