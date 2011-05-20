#include "medAttachedData.h"

class medAttachedDataPrivate
{
public:
};

medAttachedData::medAttachedData( medAttachedData *parent /*= 0*/ ) : dtkAbstractData(parent), d(new medAttachedDataPrivate)
{

}

medAttachedData::~medAttachedData( void )
{
    delete d;
    d = NULL;
}
