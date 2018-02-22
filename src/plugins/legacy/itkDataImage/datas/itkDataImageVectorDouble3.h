/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkVector.h>
#include "itkDataImage.h"
#include "itkDataImagePluginExport.h"

extern const char itkDataImageVectorDouble3Name[] = "itkDataImageVectorDouble3";
typedef itk::Vector<double,3>        DoubleVectorType;
typedef ITKDATAIMAGEPLUGIN_EXPORT itkDataImage<3,itk::Vector<double,3>,itkDataImageVectorDouble3Name> itkDataImageVectorDouble3;


