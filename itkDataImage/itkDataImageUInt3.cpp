#include "itkDataImageUInt3.h"

#include "itkDataImagePrivate.h"

#include <dtkCore/dtkAbstractDataFactory.h>

itkDataImageUInt3::itkDataImageUInt3() 
    : itkDataImageBase( "itkDataImageUInt3", new itkDataImagePrivate< char, 3 >(this) )
{
}

itkDataImageUInt3::~itkDataImageUInt3()
{
}

dtkAbstractData *createitkDataImageUInt3(void)
{
    return new itkDataImageUInt3;
}

bool itkDataImageUInt3::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataType("itkDataImageUInt3", createitkDataImageUInt3);
}

