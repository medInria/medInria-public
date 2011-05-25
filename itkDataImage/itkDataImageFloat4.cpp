#include "itkDataImageFloat4.h"

#include "itkDataImagePrivate.h"

#include <dtkCore/dtkAbstractDataFactory.h>

itkDataImageFloat4::itkDataImageFloat4() 
    : itkDataImageBase( "itkDataImageFloat4", new itkDataImagePrivate< float, 4 >(this) )
{
}

itkDataImageFloat4::~itkDataImageFloat4()
{
}

dtkAbstractData *createitkDataImageFloat4(void)
{
    return new itkDataImageFloat4;
}

bool itkDataImageFloat4::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataType("itkDataImageFloat4", createitkDataImageFloat4);
}

