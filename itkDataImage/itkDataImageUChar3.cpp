#include "itkDataImageUChar3.h"

#include "itkDataImagePrivate.h"

#include <dtkCore/dtkAbstractDataFactory.h>

itkDataImageUChar3::itkDataImageUChar3() 
    : itkDataImageBase( "itkDataImageUChar3", new itkDataImagePrivate< unsigned char, 3 >(this) )
{
}

itkDataImageUChar3::~itkDataImageUChar3()
{
}

dtkAbstractData *createitkDataImageUChar3(void)
{
    return new itkDataImageUChar3;
}

bool itkDataImageUChar3::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataType("itkDataImageUChar3", createitkDataImageUChar3);
}

