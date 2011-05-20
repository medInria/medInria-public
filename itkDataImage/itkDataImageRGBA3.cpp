#include "itkDataImageRGBA3.h"

#include "itkVectorDataImagePrivate.h"

#include <dtkCore/dtkAbstractDataFactory.h>

typedef itk::RGBAPixel<unsigned char>  RGBAPixelType;

itkDataImageRGBA3::itkDataImageRGBA3() 
    : itkDataImageBase( "itkDataImageRGBA3", new itkVectorDataImagePrivate< RGBAPixelType, 3 >(this) )
{
}

itkDataImageRGBA3::~itkDataImageRGBA3()
{
}

dtkAbstractData *createitkDataImageRGBA3(void)
{
    return new itkDataImageRGBA3;
}

bool itkDataImageRGBA3::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataType("itkDataImageRGBA3", createitkDataImageRGBA3);
}

