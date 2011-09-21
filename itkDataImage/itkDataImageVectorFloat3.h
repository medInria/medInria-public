#ifndef _itk_DataImageVectorFloat3_h_
#define _itk_DataImageVectorFloat3_h_

#include <itkVector.h>
#include "itkDataImage.h"
#include "itkDataImagePluginExport.h"

extern const char itkDataImageVectorFloat3Name[] = "itkDataImageVectorFloat3";
typedef itk::Vector<float,3>        FloatVectorType;
typedef ITKDATAIMAGEPLUGIN_EXPORT itkDataImage<3,itk::Vector<float,3>,itkDataImageVectorFloat3Name> itkDataImageVectorFloat3;

#endif
