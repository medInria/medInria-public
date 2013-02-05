/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: itkImageRegistrationFactory.cxx 1 2008-01-22 19:01:33Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2008-01-22 20:01:33 +0100 (Tue, 22 Jan 2008) $
Version:   $Revision: 1 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkImageRegistrationFactory_cxx
#define _itkImageRegistrationFactory_cxx

#include "itkImageRegistrationFactory.h"

namespace itk
{

/*
 * Constructor
 */
template <typename TImage>
ImageRegistrationFactory<TImage>
::ImageRegistrationFactory()
{
  m_InterpolationStyle = TRILINEAR;
}


/*
 * Generate Data
 */
template <typename TImage>
void
ImageRegistrationFactory<TImage>
::GenerateData()
{
  std::cout<<"generating data for image factory"<<std::endl;

  if (!this->CheckInputs())
    itkExceptionMacro(<<"inputs not set correctly !");

  if (this->GetSkipResampling())
  {
    // graft the output to keep the pipeline set
    // instead of a bad deepcopy
    typename ImageType::Pointer output = const_cast<ImageType* >(this->GetOutput());
    output->Graft (this->GetMovingImage());
    output->Update();
    return;
  }

//   this->UpdateLargestPossibleRegion();

  // instanciate the resampling process here
  // to prevent some pipeline changes problems
  typename ResampleImageFilterType::Pointer m_GeneralTransformImageFilter = ResampleImageFilterType::New();
  m_GeneralTransformImageFilter->UseReferenceImageOn();
  m_GeneralTransformImageFilter->SetTransform (this->GetGeneralTransform());
  // instanciate callback to follow the resampler
  typename CallbackType::Pointer callback = CallbackType::New();
  m_GeneralTransformImageFilter->AddObserver (itk::ProgressEvent(), callback);
  callback->SetItkObjectToWatch (this);
  // set up the resampler inputs
  m_GeneralTransformImageFilter->SetInput (this->GetMovingImage());
  // set the interpolation style for resampling
  switch (m_InterpolationStyle)
  {
      case BSPLINE:
	m_GeneralTransformImageFilter->SetInterpolator ( BSplineInterpolateFunctionType::New() );
	break;
      case NEIGHBOR:
	m_GeneralTransformImageFilter->SetInterpolator ( NeighborInterpolateFunctionType::New() );
	break;
      case SINCARD:
	m_GeneralTransformImageFilter->SetInterpolator ( SinCardInterpolateFunctionType::New() );
	break;
      default:
	m_GeneralTransformImageFilter->SetInterpolator ( LinearInterpolateFunctionType::New() );
	break;
  }

  if (this->GetUseFixedImage())
  {
    m_GeneralTransformImageFilter->SetReferenceImage (const_cast<ImageType*>(this->GetFixedImage()));
  }
  else
  {
    m_GeneralTransformImageFilter->SetReferenceImage (const_cast<ImageType*>(this->GetMovingImage()));
  }

  this->UpdateProgress (0);

  try
  {
    m_GeneralTransformImageFilter->Update();
  }
  catch (itk::ExceptionObject &e)
  {
    std::cerr<<e<<std::endl;

    this->UpdateProgress (1);
    this->SetProgress (0);
    itkExceptionMacro(<<"Error in  ImageRegistrationFactory<TImage>::GenerateData()");
    return;

  }

  this->UpdateProgress (1);
  this->SetProgress (0);

  // graft the output to keep the pipeline set
  // instead of a bad deepcopy
  typename ImageType::Pointer output = const_cast<ImageType* >(this->GetOutput());
  output->Graft (m_GeneralTransformImageFilter->GetOutput());
//   output->Update();

}




} // end namespace itk


#endif
