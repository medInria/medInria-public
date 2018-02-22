/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkRGBPixel.h>
#include <itkDataImage.h>
#include <itkDataImagePluginExport.h>

extern const char itkDataImageRGB3Name[] = "itkDataImageRGB3";
typedef itk::RGBPixel<unsigned char>     RGBPixelType;
typedef ITKDATAIMAGEPLUGIN_EXPORT itkDataImage<3,RGBPixelType,itkDataImageRGB3Name> itkDataImageRGB3;


