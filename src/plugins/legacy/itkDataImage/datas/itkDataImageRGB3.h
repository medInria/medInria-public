#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkRGBPixel.h>
#include <itkDataImage.h>
#include <itkDataImagePluginExport.h>

extern const char itkDataImageRGB3Name[] = "itkDataImageRGB3";
typedef itk::RGBPixel<unsigned char>     RGBPixelType;
typedef ITKDATAIMAGEPLUGIN_EXPORT itkDataImage<3,RGBPixelType,itkDataImageRGB3Name> itkDataImageRGB3;


