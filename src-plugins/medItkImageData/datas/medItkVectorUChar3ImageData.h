/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkVector.h>
#include <medItkImageData.h>
#include <medItkImageExport.h>

extern const char medItkVectorUChar3ImageDataName[] = "medItkVectorUChar3ImageData";
typedef itk::Vector<unsigned char,3>        UCharVectorType;
typedef MEDITKIMAGEPLUGIN_EXPORT medItkImageData<3,itk::Vector<unsigned char,3>,medItkVectorUChar3ImageDataName> medItkVectorUChar3ImageData;


