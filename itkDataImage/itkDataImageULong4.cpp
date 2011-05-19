#include "itkDataImageULong4.h"

#include "itkDataImagePrivate.h"

#include <dtkCore/dtkAbstractDataFactory.h>

itkDataImageULong4::itkDataImageULong4() 
    : itkDataImageBase( "itkDataImageULong4", new itkDataImagePrivate< unsigned long, 4 >(this) )
{
}

itkDataImageULong4::~itkDataImageULong4()
{
}

dtkAbstractData *createitkDataImageULong4(void)
{
    return new itkDataImageULong4;
}

bool itkDataImageULong4::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataType("itkDataImageULong4", createitkDataImageULong4);
}

