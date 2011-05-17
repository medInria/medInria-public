#include "itkDataImageDouble4.h"

#include "itkDataImagePrivate.h"

#include <dtkCore/dtkAbstractDataFactory.h>

itkDataImageDouble4::itkDataImageDouble4() 
    : itkDataImageBase( "itkDataImageDouble4", new itkDataImagePrivate< char, 3 >(this) )
{
}

itkDataImageDouble4::~itkDataImageDouble4()
{
}

dtkAbstractData *createitkDataImageDouble4(void)
{
    return new itkDataImageDouble4;
}

bool itkDataImageDouble4::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataType("itkDataImageDouble4", createitkDataImageDouble4);
}

