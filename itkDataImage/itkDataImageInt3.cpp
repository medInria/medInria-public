#include "itkDataImageInt3.h"

#include "itkDataImagePrivate.h"

#include <dtkCore/dtkAbstractDataFactory.h>

itkDataImageInt3::itkDataImageInt3() 
    : itkDataImageBase( "itkDataImageInt3", new itkDataImagePrivate< int, 3 >(this) )
{
}

itkDataImageInt3::~itkDataImageInt3()
{
}

dtkAbstractData *createitkDataImageInt3(void)
{
    return new itkDataImageInt3;
}

bool itkDataImageInt3::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataType("itkDataImageInt3", createitkDataImageInt3);
}

