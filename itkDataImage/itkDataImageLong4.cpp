#include "itkDataImageLong4.h"

#include "itkDataImagePrivate.h"

#include <dtkCore/dtkAbstractDataFactory.h>

itkDataImageLong4::itkDataImageLong4() 
    : itkDataImageBase( "itkDataImageLong4", new itkDataImagePrivate< long, 4 >(this) )
{
}

itkDataImageLong4::~itkDataImageLong4()
{
}

dtkAbstractData *createitkDataImageLong4(void)
{
    return new itkDataImageLong4;
}

bool itkDataImageLong4::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataType("itkDataImageLong4", createitkDataImageLong4);
}

