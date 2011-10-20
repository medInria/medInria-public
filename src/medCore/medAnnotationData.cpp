#include "medAnnotationData.h"

#include <medCore/medAbstractData.h>

#include <dtkCore/dtkSmartPointer.h>

class medAnnotationData::medAnnotationDataPrivate
{
public:
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







