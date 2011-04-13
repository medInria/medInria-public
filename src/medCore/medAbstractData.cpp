#include "medAbstractData.h"

class medAbstractDataPrivate
{
public:
};

medAbstractData::medAbstractData( medAbstractData *parent /*= 0*/ ) : dtkAbstractData(parent), d(new medAbstractDataPrivate)
{

}

medAbstractData::medAbstractData( const medAbstractData& data )
{
    this->setParent(data.parent());
    dtkAbstractData::dtkAbstractData(data);
}

medAbstractData::~medAbstractData( void )
{
    delete d;
    d = NULL;
}
