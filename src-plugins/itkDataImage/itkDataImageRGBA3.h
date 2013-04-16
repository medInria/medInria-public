/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkRGBAPixel.h>
#include "itkDataImage.h"
#include "itkDataImagePluginExport.h"

extern const char itkDataImageRGBA3Name[] = "itkDataImageRGBA3";
typedef itk::RGBAPixel<unsigned char>  RGBAPixelType;
typedef ITKDATAIMAGEPLUGIN_EXPORT itkDataImage<3,RGBAPixelType,itkDataImageRGBA3Name> itkDataImageRGBA3;


