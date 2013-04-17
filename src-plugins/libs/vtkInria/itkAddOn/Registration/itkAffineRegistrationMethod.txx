/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "itkAffineRegistrationMethod.h"
#include "itkAffineTransform.h"
#include "itkTranslationTransform.h"


namespace itk
{

/*
 * Constructor
 */
template <typename TImage>
AffineRegistrationMethod<TImage>
::AffineRegistrationMethod()
{

  typename LinearInterpolatorType::Pointer m_LinearInterpolator = LinearInterpolatorType::New();
  
  this->Parameters.Metric = NULL; //bad
  this->Parameters.Interpolator = m_LinearInterpolator;
  this->Parameters.OptimizationScale = 1.0/10000.0;
  this->Parameters.NumberOfLevels = 3;
  this->Parameters.NumberOfIterationList = new unsigned int[100];
  for (unsigned int i=0; i<100; i++)
    this->Parameters.NumberOfIterationList[i] = 10;
  
  this->SetName ("Affine Automatic");

  m_AutoPyramidSchedule = 1;
  ScheduleType nullschedule (0,0);
  m_Schedule = nullschedule;
}

  
/*
 * Constructor
 */
template <typename TImage>
void
AffineRegistrationMethod<TImage>
::Initialize()
{
  this->Superclass::Initialize ();
}

  

/*
 * PrintSelf
 */
template <typename TImage>
void
AffineRegistrationMethod<TImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf( os, indent );
}





  
/*
 * Generate Data
 */
template <typename TImage>
void
AffineRegistrationMethod<TImage>
::GenerateData()
{
  
  typename AffineTransformType::Pointer transform = AffineTransformType::New();
  transform->SetIdentity();


  typename OptimizerType::Pointer m_Optimizer = OptimizerType::New();
  typename RegistrationType::Pointer m_Registration = RegistrationType::New();
  typename CommandType::Pointer m_Callback = CommandType::New();
  typename ImagePyramidType::Pointer m_FixedImagePyramid = ImagePyramidType::New();
  typename ImagePyramidType::Pointer m_MovingImagePyramid = ImagePyramidType::New();
  typename MutualInformationMetricType::Pointer m_MutualInformationMetric = MutualInformationMetricType::New();

  m_MutualInformationMetric->SetNumberOfHistogramBins( 20 );
  int samples = this->roundint(1.0/this->Parameters.OptimizationScale);
  m_MutualInformationMetric->SetNumberOfSpatialSamples( samples );
  m_MutualInformationMetric->ReinitializeSeed( 76926294 );

  if (m_AutoPyramidSchedule)
  {
    this->SetSchedule (this->GetOptimumSchedule(8, this->Parameters.NumberOfLevels));
  }
  
//   m_FixedImagePyramid->SetSchedule (this->GetSchedule());
//   m_MovingImagePyramid->SetSchedule (this->GetSchedule());
  
  
  this->Parameters.Metric = m_MutualInformationMetric;
  
  // set registration parameters
  m_Optimizer->SetNumberOfIterations( this->Parameters.NumberOfIterationList[0]);  
  m_Optimizer->MinimizeOn();
  
  unsigned int N = ImageType::ImageDimension * (ImageType::ImageDimension + 1);
  OptimizerScalesType optimizerScales( N );
  for (unsigned int i = 0; i < N - ImageType::ImageDimension; i++)
    optimizerScales[i] =  1.0;
  for (unsigned int i = N - ImageType::ImageDimension; i < N; i++)
    optimizerScales[i] =  this->Parameters.OptimizationScale;

  m_Optimizer->SetScales (optimizerScales);
  m_Optimizer->SetMaximumStepLength (4);
  
  m_Registration->SetMetric( this->Parameters.Metric );
  m_Registration->SetOptimizer( m_Optimizer );
  m_Registration->SetInterpolator( this->Parameters.Interpolator  );
  m_Registration->SetFixedImagePyramid (m_FixedImagePyramid);
  m_Registration->SetMovingImagePyramid (m_MovingImagePyramid);

  m_Registration->SetSchedules (this->GetSchedule(), this->GetSchedule());
  
//   m_Registration->SetNumberOfLevels( this->Parameters.NumberOfLevels );
  
  m_Callback->SetItkObjectToWatch (this);
  m_Optimizer->AddObserver(itk::IterationEvent(), m_Callback);
  m_Registration->AddObserver(itk::IterationEvent(), m_Callback);
  
  m_Registration->SetTransform( transform );
  m_Registration->SetInitialTransformParameters (transform->GetParameters());
  m_Registration->SetFixedImage( this->GetFixedImage() );
  m_Registration->SetMovingImage( this->GetMovingImage() );
  m_Registration->SetFixedImageRegion( this->GetFixedImage()->GetLargestPossibleRegion() );

  std::vector<unsigned int> iterations;
  
  for (unsigned int i=0; i<m_Registration->GetNumberOfLevels(); i++)
  {
    iterations.push_back (this->Parameters.NumberOfIterationList[i]);
  }  
  m_Callback->SetNumberOfIterations (iterations);

  
  m_Registration->UpdateProgress (0);
  this->UpdateProgress(0.0);

  try 
  {
    m_Registration->StartRegistration();  
  } 
  catch( itk::ExceptionObject & e ) 
  { 
    this->UpdateProgress(1.0);
    this->SetProgress(0.0);
    std::cerr << e << std::endl; 
    throw itk::ExceptionObject(__FILE__,__LINE__,"Error in AffineRegistrationMethod<TImage>::GenerateData()");
  }

  this->UpdateProgress(1.0);
  this->SetProgress(0.0);

  this->SetTransform (transform);
  
}

    
/*
 * metric parameter
 */
template <typename TImage>
void
AffineRegistrationMethod<TImage>
::SetRegistrationMetric (MetricPointerType metric)
{
  this->Parameters.Metric = metric;
}
  
    
/*
 * interpolation parameter
 */
template <typename TImage>
void
AffineRegistrationMethod<TImage>
::SetRegistrationInterpolator (InterpolatorPointerType interpolator)
{
  this->Parameters.Interpolator = interpolator;
}

/*
 * levels parameter
 */
template <typename TImage>
unsigned int
AffineRegistrationMethod<TImage>
::GetRegistrationNumberOfLevels() const
{
  return this->Parameters.NumberOfLevels;
}


/*
 * levels parameter
 */
template <typename TImage>
void
AffineRegistrationMethod<TImage>
::SetRegistrationNumberOfLevels (unsigned int n)
{
  this->Parameters.NumberOfLevels = n;
}

/*
 * iterations parameter
 */
template <typename TImage>
const unsigned int*
AffineRegistrationMethod<TImage>
::GetRegistrationNumberOfIterationsList() const
{
  
  return this->Parameters.NumberOfIterationList;
}

/*
 * iterations parameter
 */
template <typename TImage>
void
AffineRegistrationMethod<TImage>
::SetRegistrationNumberOfIterationsList (unsigned int* n_list)
{
  for (unsigned int i=0; i<this->Parameters.NumberOfLevels; i++)
  {
    this->Parameters.NumberOfIterationList[i] = n_list[i];
  }
}

/*
 * translation parameter
 */
template <typename TImage>
double
AffineRegistrationMethod<TImage>
::GetTranslationScale() const
{
  return this->Parameters.OptimizationScale;
}


/*
 * translation parameter
 */
template <typename TImage>
void
AffineRegistrationMethod<TImage>
::SetTranslationScale (double scale)
{
  this->Parameters.OptimizationScale = scale;
}

/*
 * schedule
 */
template <typename TImage>
void 
AffineRegistrationMethod<TImage>
::SetSchedule (ScheduleType schedule)
{
  m_AutoPyramidSchedule = 0;
  m_Schedule = schedule;
  this->SetRegistrationNumberOfLevels (schedule.rows());
}


  

} // end namespace itk



