#ifndef _itk_DataImageVector3_h_
#define _itk_DataImageVector3_h_

#include <itkVector.h>
#include "itkDataImage.h"
#include "itkDataImagePluginExport.h"

extern const char itkDataImageVector3Name[] = "itkDataImageVector3";
typedef itk::Vector<unsigned char,3>        UCharVectorType;
typedef ITKDATAIMAGEPLUGIN_EXPORT itkDataImage<3,itk::Vector<unsigned char,3>,itkDataImageVector3Name> itkDataImageVector3;

#endif
