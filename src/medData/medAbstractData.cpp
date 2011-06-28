#include "medAbstractData.h"

class medAbstractDataPrivate
{
public:
    bool isTrueVolumetric;
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

bool medAbstractData::trueVolumetric()
{
    return d->isTrueVolumetric;
}
