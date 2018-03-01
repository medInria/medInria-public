/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkGISImageIOFactory.h>
#include <itkCreateObjectFunction.h>
#include <itkGISImageIO.h>
#include <itkVersion.h>

  
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
