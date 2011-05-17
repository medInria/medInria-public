#include "itkDataImageFloat3.h"

#include "itkDataImagePrivate.h"

#include <dtkCore/dtkAbstractDataFactory.h>

itkDataImageFloat3::itkDataImageFloat3() 
    : itkDataImageBase( "itkDataImageFloat3", new itkDataImagePrivate< char, 3 >(this) )
{
}

itkDataImageFloat3::~itkDataImageFloat3()
{
}

dtkAbstractData *createitkDataImageFloat3(void)
{
    return new itkDataImageFloat3;
}

bool itkDataImageFloat3::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataType("itkDataImageFloat3", createitkDataImageFloat3);
}

