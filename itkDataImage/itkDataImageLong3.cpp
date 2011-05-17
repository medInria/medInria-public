#include "itkDataImageLong3.h"

#include "itkDataImagePrivate.h"

#include <dtkCore/dtkAbstractDataFactory.h>

itkDataImageLong3::itkDataImageLong3() 
    : itkDataImageBase( "itkDataImageLong3", new itkDataImagePrivate< char, 3 >(this) )
{
}

itkDataImageLong3::~itkDataImageLong3()
{
}

dtkAbstractData *createitkDataImageLong3(void)
{
    return new itkDataImageLong3;
}

bool itkDataImageLong3::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataType("itkDataImageLong3", createitkDataImageLong3);
}

