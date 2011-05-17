#include "itkDataImageChar3.h"

#include "itkDataImagePrivate.h"

#include <dtkCore/dtkAbstractDataFactory.h>

itkDataImageChar3::itkDataImageChar3() 
    : itkDataImageBase( "itkDataImageChar3", new itkDataImagePrivate< char, 3 >(this) )
{
}

itkDataImageChar3::~itkDataImageChar3()
{
}

dtkAbstractData *createitkDataImageChar3(void)
{
    return new itkDataImageChar3;
}

bool itkDataImageChar3::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataType("itkDataImageChar3", createitkDataImageChar3);
}

