#pragma once

#include "itkDataImage.h"
#include "itkDataImagePluginExport.h"

extern const char itkDataImageUChar3Name[] = "itkDataImageUChar3";
typedef ITKDATAIMAGEPLUGIN_EXPORT itkDataImage<3,unsigned char,itkDataImageUChar3Name> itkDataImageUChar3;


