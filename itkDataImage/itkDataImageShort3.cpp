#include "itkDataImageShort3.h"

#include "itkDataImagePrivate.h"

#include <dtkCore/dtkAbstractDataFactory.h>

itkDataImageShort3::itkDataImageShort3() 
    : itkDataImageBase( "itkDataImageShort3", new itkDataImagePrivate< char, 3 >(this) )
{
}

itkDataImageShort3::~itkDataImageShort3()
{
}

dtkAbstractData *createitkDataImageShort3(void)
{
    return new itkDataImageShort3;
}

bool itkDataImageShort3::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataType("itkDataImageShort3", createitkDataImageShort3);
}

