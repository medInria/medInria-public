#include "medAttachedData.h"

class medAttachedDataPrivate
{
public:
};

medAttachedData::medAttachedData( medAttachedData *parent /*= 0*/ ) : dtkAbstractData(parent), d(new medAttachedDataPrivate)
{

}

medAttachedData::medAttachedData( const medAttachedData& data )
{
    this->setParent(data.parent());
    dtkAbstractData::dtkAbstractData(data);
}

medAttachedData::~medAttachedData( void )
{
    delete d;
    d = NULL;
}
