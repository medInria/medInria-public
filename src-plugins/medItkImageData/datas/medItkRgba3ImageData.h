/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkRGBAPixel.h>
#include <medItkImageData.h>
#include <medItkImageExport.h>

extern const char medItkRgba3ImageDataName[] = "medItkRgba3ImageData";
typedef itk::RGBAPixel<unsigned char>  RGBAPixelType;
typedef MEDITKIMAGEPLUGIN_EXPORT medItkImageData<3,RGBAPixelType,medItkRgba3ImageDataName> medItkRgba3ImageData;


