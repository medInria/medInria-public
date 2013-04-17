/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkImageRegistrationFactory.h"
#include "itkImageRegistrationFactory.txx"



namespace itk
{

  template class ImageRegistrationFactory< itk::Image<float,3> >;
  template class ImageRegistrationFactory< itk::Image<unsigned int,3> >;
  template class ImageRegistrationFactory< itk::Image<short,3> >;
  
}
