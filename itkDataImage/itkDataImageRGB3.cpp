#include "medItk.h"

#include "itkDataImageRGB3.h"

#include "medITKDataImageMacros.h"

typedef itk::RGBPixel<unsigned char>  RGBPixelType;

medImplementITKVectorDataImage (RGBPixelType, 3, RGB3);
