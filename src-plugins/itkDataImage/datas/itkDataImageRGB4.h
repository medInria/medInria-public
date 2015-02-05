/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkRGBPixel.h>
#include <itkDataImage.h>
#include <itkDataImagePluginExport.h>

extern const char itkDataImageRGB4Name[] = "itkDataImageRGB4";
typedef itk::RGBPixel<unsigned char>     RGBPixelType;
typedef ITKDATAIMAGEPLUGIN_EXPORT itkDataImage<4,RGBPixelType,itkDataImageRGB4Name> itkDataImageRGB4;


