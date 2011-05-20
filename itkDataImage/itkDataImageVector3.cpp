#include "itkDataImageVector3.h"

#include "itkVectorDataImagePrivate.h"

#include <dtkCore/dtkAbstractDataFactory.h>

typedef itk::Vector<unsigned char, 3> UCharVectorType;

itkDataImageVector3::itkDataImageVector3() 
    : itkDataImageBase( "itkDataImageVector3", new itkVectorDataImagePrivate< UCharVectorType, 3 >(this) )
{
}

itkDataImageVector3::~itkDataImageVector3()
{
}

dtkAbstractData *createitkDataImageVector3(void)
{
    return new itkDataImageVector3;
}

bool itkDataImageVector3::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataType("itkDataImageVector3", createitkDataImageVector3);
}

