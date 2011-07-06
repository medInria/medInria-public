#ifndef _itk_DataImageRBGA3_h_
#define _itk_DataImageRBGA3_h_

#include <itkRGBAPixel.h>
#include "itkDataImage.h"
#include "itkDataImagePluginExport.h"

extern const char itkDataImageRGBA3Name[] = "itkDataImageRGBA3";
typedef itk::RGBAPixel<unsigned char>  RGBAPixelType;
typedef ITKDATAIMAGEPLUGIN_EXPORT itkDataImage<3,RGBAPixelType,itkDataImageRGBA3Name> itkDataImageRGBA3;

#endif
