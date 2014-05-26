/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkVector.h>
#include <itkDataImage.h>
#include <itkDataImagePluginExport.h>

extern const char itkDataImageVectorFloat3Name[] = "itkDataImageVectorFloat3";
typedef itk::Vector<float,3>        FloatVectorType;
typedef ITKDATAIMAGEPLUGIN_EXPORT itkDataImage<3,itk::Vector<float,3>,itkDataImageVectorFloat3Name> itkDataImageVectorFloat3;


