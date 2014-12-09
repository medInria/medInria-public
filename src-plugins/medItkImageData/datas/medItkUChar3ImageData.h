/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medItkImageData.h>
#include <medItkImageExport.h>

extern const char medItkUChar3ImageDataName[] = "medItkUChar3ImageData";
typedef MEDITKIMAGEPLUGIN_EXPORT medItkImageData<3,unsigned char,medItkUChar3ImageDataName> medItkUChar3ImageData;


