#include "itkDataImageRGBA3.h"

#include "medITKDataImageMacros.h"

typedef itk::RGBAPixel<unsigned char>  RGBAPixelType;

medImplementITKVectorDataImage (RGBAPixelType, 3, RGBA3);
