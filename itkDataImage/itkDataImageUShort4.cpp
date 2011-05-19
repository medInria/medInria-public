#include "itkDataImageUShort4.h"

#include "itkDataImagePrivate.h"

#include <dtkCore/dtkAbstractDataFactory.h>

itkDataImageUShort4::itkDataImageUShort4() 
    : itkDataImageBase( "itkDataImageUShort4", new itkDataImagePrivate< unsigned short, 4 >(this) )
{
}

itkDataImageUShort4::~itkDataImageUShort4()
{
}

dtkAbstractData *createitkDataImageUShort4(void)
{
    return new itkDataImageUShort4;
}

bool itkDataImageUShort4::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataType("itkDataImageUShort4", createitkDataImageUShort4);
}

