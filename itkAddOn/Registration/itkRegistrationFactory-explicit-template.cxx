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
#include "itkImageRegistrationFactory.h"
#include "itkImageRegistrationFactory.txx"



namespace itk
{

  template class ImageRegistrationFactory< itk::Image<float,3> >;
  template class ImageRegistrationFactory< itk::Image<unsigned int,3> >;
  template class ImageRegistrationFactory< itk::Image<short,3> >;
  
}
