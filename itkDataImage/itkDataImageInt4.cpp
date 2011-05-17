#include "itkDataImageInt4.h"

#include "itkDataImagePrivate.h"

#include <dtkCore/dtkAbstractDataFactory.h>

itkDataImageInt4::itkDataImageInt4() 
    : itkDataImageBase( "itkDataImageInt4", new itkDataImagePrivate< char, 3 >(this) )
{
}

itkDataImageInt4::~itkDataImageInt4()
{
}

dtkAbstractData *createitkDataImageInt4(void)
{
    return new itkDataImageInt4;
}

bool itkDataImageInt4::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataType("itkDataImageInt4", createitkDataImageInt4);
}

