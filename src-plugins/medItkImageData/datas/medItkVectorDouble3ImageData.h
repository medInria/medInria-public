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
#include "medItkImageData.h"
#include "medItkImageExport.h"

extern const char medItkVectorDouble3ImageDataName[] = "medItkVectorDouble3ImageData";
typedef itk::Vector<double,3>        DoubleVectorType;
typedef MEDITKIMAGEPLUGIN_EXPORT medItkImageData<3,itk::Vector<double,3>,medItkVectorDouble3ImageDataName> medItkVectorDouble3ImageData;


