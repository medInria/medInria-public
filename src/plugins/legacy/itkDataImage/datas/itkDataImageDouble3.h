#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDataImage.h>
#include <itkDataImagePluginExport.h>

extern const char itkDataImageDouble3Name[] = "itkDataImageDouble3";
typedef ITKDATAIMAGEPLUGIN_EXPORT itkDataImage<3,double,itkDataImageDouble3Name> itkDataImageDouble3;


