#include "itkDataImageUInt4.h"

#include "itkDataImagePrivate.h"

#include <dtkCore/dtkAbstractDataFactory.h>

itkDataImageUInt4::itkDataImageUInt4() 
    : itkDataImageBase( "itkDataImageUInt4", new itkDataImagePrivate< unsigned int, 4 >(this) )
{
}

itkDataImageUInt4::~itkDataImageUInt4()
{
}

dtkAbstractData *createitkDataImageUInt4(void)
{
    return new itkDataImageUInt4;
}

bool itkDataImageUInt4::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataType("itkDataImageUInt4", createitkDataImageUInt4);
}

