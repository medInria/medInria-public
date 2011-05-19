#include "itkDataImageUChar4.h"

#include "itkDataImagePrivate.h"

#include <dtkCore/dtkAbstractDataFactory.h>

itkDataImageUChar4::itkDataImageUChar4() 
    : itkDataImageBase( "itkDataImageUChar4", new itkDataImagePrivate< unsigned char, 4 >(this) )
{
}

itkDataImageUChar4::~itkDataImageUChar4()
{
}

dtkAbstractData *createitkDataImageUChar4(void)
{
    return new itkDataImageUChar4;
}

bool itkDataImageUChar4::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataType("itkDataImageUChar4", createitkDataImageUChar4);
}

