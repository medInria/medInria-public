/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkDataImage.h>
#include <itkDataImagePluginExport.h>

extern const char itkDataImageDouble4Name[] = "itkDataImageDouble4";
typedef ITKDATAIMAGEPLUGIN_EXPORT itkDataImage<4,double,itkDataImageDouble4Name> itkDataImageDouble4;


