#include "itkDataImageUShort3.h"

#include "itkDataImagePrivate.h"

#include <dtkCore/dtkAbstractDataFactory.h>

itkDataImageUShort3::itkDataImageUShort3() 
    : itkDataImageBase( "itkDataImageUShort3", new itkDataImagePrivate< char, 3 >(this) )
{
}

itkDataImageUShort3::~itkDataImageUShort3()
{
}

dtkAbstractData *createitkDataImageUShort3(void)
{
    return new itkDataImageUShort3;
}

bool itkDataImageUShort3::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataType("itkDataImageUShort3", createitkDataImageUShort3);
}

