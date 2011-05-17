#include "itkDataImageULong3.h"

#include "itkDataImagePrivate.h"

#include <dtkCore/dtkAbstractDataFactory.h>

itkDataImageULong3::itkDataImageULong3() 
    : itkDataImageBase( "itkDataImageULong3", new itkDataImagePrivate< char, 3 >(this) )
{
}

itkDataImageULong3::~itkDataImageULong3()
{
}

dtkAbstractData *createitkDataImageULong3(void)
{
    return new itkDataImageULong3;
}

bool itkDataImageULong3::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataType("itkDataImageULong3", createitkDataImageULong3);
}

