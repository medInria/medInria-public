/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "itkRigidRegistrationMethod.h"

namespace itk
{


template <typename TImage>
RigidRegistrationMethod<TImage>
::RigidRegistrationMethod()
{
	m_metric        = MetricType::New();
	m_transform     = TransformType::New();
	m_optimizer     = OptimizerType::New();
	m_interpolator  = InterpolatorType::New();
	m_registration  = RegistrationType::New();
}

  

template <typename TImage>
void
RigidRegistrationMethod<TImage>
::Initialize()
{
  this->Superclass::Initialize ();
}

  

template <typename TImage>
void
RigidRegistrationMethod<TImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf( os, indent );
}



/*
 * Generate Data
 */
template <typename TImage>
void
RigidRegistrationMethod<TImage>
::GenerateData()
{

  m_registration->SetMetric(        m_metric        );
  m_registration->SetOptimizer(     m_optimizer     );
  m_registration->SetTransform(     m_transform     );
  m_registration->SetInterpolator(  m_interpolator  );

  m_metric->SetNumberOfHistogramBins( 20 );
  m_metric->SetNumberOfSpatialSamples( 10000 );

  //Initialize transform
  typedef itk::CenteredTransformInitializer< TransformType,ImageType,ImageType>  TransformInitializerType;
  typename TransformInitializerType::Pointer initializer = TransformInitializerType::New();
  initializer->SetTransform(   m_transform );
  initializer->SetFixedImage(  this->GetFixedImage() );
  initializer->SetMovingImage( this->GetMovingImage() );
  initializer->GeometryOn() ;
  //initializer->MomentsOn();
  initializer->InitializeTransform();

  m_registration->SetInitialTransformParameters( m_transform->GetParameters() );
 
  // set registration parameters
  
  // Initialize optimizer
  m_optimizer->SetStepLength( 1 );
  m_optimizer->SetMaximumIteration( 200 );
  m_optimizer->SetStepTolerance( 0.01 );
  m_optimizer->SetMaximize(false);
  m_optimizer->SetValueTolerance( 0.1 );

  /* BIG TODO : INCORPORATE NICE ITK CALLBACKS WHERE NEEDED */

  m_registration->SetFixedImage( this->GetFixedImage() );
  m_registration->SetMovingImage( this->GetMovingImage() );
  m_registration->SetFixedImageRegion( this->GetFixedImage()->GetLargestPossibleRegion() );

  
  m_registration->UpdateProgress (0);
  this->UpdateProgress(0.0);

  try 
  {
	std::cout << "Launching registration ... " << std::endl;
	m_registration->StartRegistration();  
  } 
  catch( itk::ExceptionObject & e ) 
  { 
    this->UpdateProgress(1.0);
    this->SetProgress(0.0);
    std::cerr << e << std::endl; 
    throw itk::ExceptionObject(__FILE__,__LINE__,"Error in RigidRegistrationMethod<TImage>::GenerateData()");
  }

  typename OptimizerType::ParametersType finalParameters = m_registration->GetLastTransformParameters();
  const double finalRotationAboutX  = finalParameters[0];
  const double finalRotationAboutY  = finalParameters[1];
  const double finalRotationAboutZ  = finalParameters[2];
  const double finalTranslationX    = finalParameters[3];
  const double finalTranslationY    = finalParameters[4];
  const double finalTranslationZ    = finalParameters[5];


  // Print out results
  std::cout << " RotationAbout X  = " << finalRotationAboutX  << std::endl;
  std::cout << " RotationAbout Y  = " << finalRotationAboutY  << std::endl;
  std::cout << " RotationAbout Z  = " << finalRotationAboutZ  << std::endl;
  std::cout << " Translation X    = " << finalTranslationX  << std::endl;
  std::cout << " Translation Y    = " << finalTranslationY  << std::endl;
  std::cout << " Translation Z    = " << finalTranslationZ  << std::endl;

  m_transform->SetParameters( finalParameters );

  this->SetTransform (m_transform);
  this->UpdateProgress(1.0);
  this->SetProgress(0.0);
}

  
} // end namespace itk



