#include "itkDataImageDouble3.h"

#include "itkDataImagePrivate.h"

#include <dtkCore/dtkAbstractDataFactory.h>

itkDataImageDouble3::itkDataImageDouble3() 
    : itkDataImageBase( "itkDataImageDouble3", new itkDataImagePrivate< double, 3 >(this) )
{
}

itkDataImageDouble3::~itkDataImageDouble3()
{
}

dtkAbstractData *createitkDataImageDouble3(void)
{
    return new itkDataImageDouble3;
}

bool itkDataImageDouble3::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataType("itkDataImageDouble3", createitkDataImageDouble3);
}

