#include "itkDataImageShort4.h"

#include "itkDataImagePrivate.h"

#include <dtkCore/dtkAbstractDataFactory.h>

itkDataImageShort4::itkDataImageShort4() 
    : itkDataImageBase( "itkDataImageShort4", new itkDataImagePrivate< short, 4 >(this) )
{
}

itkDataImageShort4::~itkDataImageShort4()
{
}

dtkAbstractData *createitkDataImageShort4(void)
{
    return new itkDataImageShort4;
}

bool itkDataImageShort4::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataType("itkDataImageShort4", createitkDataImageShort4);
}

