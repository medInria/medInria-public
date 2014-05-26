/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkDataImage.h>
#include <itkDataImagePluginExport.h>

extern const char itkDataImageULong3Name[] = "itkDataImageULong3";
typedef ITKDATAIMAGEPLUGIN_EXPORT itkDataImage<3,unsigned long,itkDataImageULong3Name> itkDataImageULong3;


