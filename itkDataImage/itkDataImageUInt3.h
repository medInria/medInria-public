#ifndef _itk_DataImageUInt3_h_
#define _itk_DataImageUInt3_h_

#include "itkDataImage.h"
#include "itkDataImagePluginExport.h"

extern const char itkDataImageUInt3Name[] = "itkDataImageUInt3";
typedef ITKDATAIMAGEPLUGIN_EXPORT itkDataImage<3,unsigned int,itkDataImageUInt3Name> itkDataImageUInt3;

#endif
