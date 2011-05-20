#include "itkDataImageRGB3.h"

#include "itkVectorDataImagePrivate.h"

#include <dtkCore/dtkAbstractDataFactory.h>

typedef itk::RGBPixel<unsigned char>  RGBPixelType;

itkDataImageRGB3::itkDataImageRGB3() 
    : itkDataImageBase( "itkDataImageRGB3", new itkVectorDataImagePrivate< RGBPixelType, 3 >(this) )
{
}

itkDataImageRGB3::~itkDataImageRGB3()
{
}

dtkAbstractData *createitkDataImageRGB3(void)
{
    return new itkDataImageRGB3;
}

bool itkDataImageRGB3::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataType("itkDataImageRGB3", createitkDataImageRGB3);
}

