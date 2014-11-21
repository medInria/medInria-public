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
#include <medItkImageData.h>
#include <medItkImageExport.h>

extern const char medItkRgb3ImageDataName[] = "medItkRgb3ImageData";
typedef itk::RGBPixel<unsigned char>     RGBPixelType;
typedef MEDITKIMAGEPLUGIN_EXPORT medItkImageData<3,RGBPixelType,medItkRgb3ImageDataName> medItkRgb3ImageData;


