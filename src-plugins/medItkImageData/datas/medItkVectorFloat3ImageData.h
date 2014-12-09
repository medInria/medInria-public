/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkVector.h>
#include <medItkImageData.h>
#include <medItkImageExport.h>

extern const char medItkVectorFloat3ImageDataName[] = "medItkVectorFloat3ImageData";
typedef itk::Vector<float,3>        FloatVectorType;
typedef MEDITKIMAGEPLUGIN_EXPORT medItkImageData<3,itk::Vector<float,3>,medItkVectorFloat3ImageDataName> medItkVectorFloat3ImageData;


