#include "itkDataImageChar4.h"

#include "itkDataImagePrivate.h"

#include <dtkCore/dtkAbstractDataFactory.h>

itkDataImageChar4::itkDataImageChar4() 
    : itkDataImageBase( "itkDataImageChar4", new itkDataImagePrivate< char, 4 >(this) )
{
}

itkDataImageChar4::~itkDataImageChar4()
{
}

dtkAbstractData *createitkDataImageChar4(void)
{
    return new itkDataImageChar4;
}

bool itkDataImageChar4::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataType("itkDataImageChar4", createitkDataImageChar4);
}

