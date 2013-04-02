#pragma once

#include <itkVector.h>
#include "itkDataImage.h"
#include "itkDataImagePluginExport.h"

extern const char itkDataImageVectorUChar3Name[] = "itkDataImageVectorUChar3";
typedef itk::Vector<unsigned char,3>        UCharVectorType;
typedef ITKDATAIMAGEPLUGIN_EXPORT itkDataImage<3,itk::Vector<unsigned char,3>,itkDataImageVectorUChar3Name> itkDataImageVectorUChar3;


