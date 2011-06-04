#ifndef _itk_DataImageRGB3_h_
#define _itk_DataImageRGB3_h_

#include <itkRGBPixel.h>
#include "itkDataImage.h"
#include "itkDataImagePluginExport.h"

extern const char itkDataImageRGB3Name[] = "itkDataImageRGB3";
typedef itk::RGBPixel<unsigned char>     RGBPixelType;
typedef ITKDATAIMAGEPLUGIN_EXPORT itkDataImage<3,RGBPixelType,itkDataImageRGB3Name> itkDataImageRGB3;

#endif
