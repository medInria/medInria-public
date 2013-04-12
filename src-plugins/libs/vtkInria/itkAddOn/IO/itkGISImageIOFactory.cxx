/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id:  1182 2009-06-30 09:19:05Z  $
Language:  C++
Author:    $Author:  $
Date:      $Date: 2009-06-30 11:19:05 +0200 (mar, 30 jun 2009) $
Version:   $Revision: 1182 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkGISImageIOFactory.h"
#include "itkCreateObjectFunction.h"
#include "itkGISImageIO.h"
#include "itkVersion.h"

  
namespace itk
{

GISImageIOFactory::GISImageIOFactory()
{
  this->RegisterOverride("itkImageIOBase",
                         "itkGISImageIO",
                         "GIS Image IO",
                         1,
                         CreateObjectFunction<GISImageIO>::New());
}
  
GISImageIOFactory::~GISImageIOFactory()
{
}

const char* 
GISImageIOFactory::GetITKSourceVersion() const
{
  return ITK_SOURCE_VERSION;
}

const char* 
GISImageIOFactory::GetDescription() const
{
  return "GIS ImageIO Factory, allows the loading of GIS images into Insight";
}

} // end namespace itk
