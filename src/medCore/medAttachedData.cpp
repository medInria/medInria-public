#include "medAttachedData.h"

class medAttachedDataPrivate
{
public:
};

medAttachedData::medAttachedData( medAttachedData * parent )
    : medAbstractData(parent)
    , d(new medAttachedDataPrivate)
{
}

medAttachedData::~medAttachedData( void )
{
    delete d;
}
