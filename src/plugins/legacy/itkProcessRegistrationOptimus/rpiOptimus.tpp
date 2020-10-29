#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkMattesMutualInformationImageToImageMetric.h>
#include <itkImageRegistrationMethod.h>
#include <itkCenteredTransformInitializer.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkNewUoaOptimizer.h>

//#include "rpiOptimus.hxx"



// Namespace RPI : Registration Programming Interface
namespace rpi {

template < class TFixedImage, class TMovingImage, class TTransformScalarType >
Optimus< TFixedImage, TMovingImage, TTransformScalarType >
::Optimus(void)
{

    // Initialize the parameters
    this->m_iterations         = 5000;
    this->m_histogramBins      = 50;
    this->m_spatialSamples     = 200000;
    this->m_interpolations     = 28;
    this->m_rhoStart           = 0.6;
    this->m_rhoEnd             = 0.003;
    this->m_scalingCoefficient = 20.0;

    // Initialize the transformations
    this->m_transform          = TransformType::New();
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
Optimus< TFixedImage, TMovingImage, TTransformScalarType >
::~Optimus(void)
{
    // Do nothing
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
typename Optimus< TFixedImage, TMovingImage, TTransformScalarType >::TransformPointerType
Optimus< TFixedImage, TMovingImage, TTransformScalarType >
::GetInitialTransformation(void) const
{
    return this->m_initialTransform;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
Optimus< TFixedImage, TMovingImage, TTransformScalarType >
::SetInitialTransformation(TransformType * transform)
{
    this->m_initialTransform = transform;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
unsigned int
Optimus< TFixedImage, TMovingImage, TTransformScalarType >
::GetNumberOfInterpolations() const
{
    return this->m_interpolations;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
Optimus< TFixedImage, TMovingImage, TTransformScalarType >
::SetNumberOfInterpolations(unsigned int value)
{
    this->m_interpolations = value;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
unsigned int
Optimus< TFixedImage, TMovingImage, TTransformScalarType >
::GetNumberOfIterations(void) const
{
    return this->m_iterations;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
Optimus< TFixedImage, TMovingImage, TTransformScalarType >
::SetNumberOfIterations(unsigned int value)
{
    this->m_iterations = value;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
unsigned int
Optimus< TFixedImage, TMovingImage, TTransformScalarType >
::GetNumberOfHistogramBins() const
{
    return this->m_histogramBins;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
Optimus< TFixedImage, TMovingImage, TTransformScalarType >
::SetNumberOfHistogramBins(unsigned int value)
{
    this->m_histogramBins = value;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
unsigned int
Optimus< TFixedImage, TMovingImage, TTransformScalarType >
::GetNumberOfSpatialSamples() const
{
    return this->m_spatialSamples;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
Optimus< TFixedImage, TMovingImage, TTransformScalarType >
::SetNumberOfSpatialSamples(unsigned int value)
{
    this->m_spatialSamples = value;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
float
Optimus< TFixedImage, TMovingImage, TTransformScalarType >
::GetRhoStart() const
{
    return this->m_rhoStart;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
Optimus< TFixedImage, TMovingImage, TTransformScalarType >
::SetRhoStart(float value)
{
    this->m_rhoStart = value;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
float
Optimus< TFixedImage, TMovingImage, TTransformScalarType >
::GetRhoEnd() const
{
    return this->m_rhoEnd;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
Optimus< TFixedImage, TMovingImage, TTransformScalarType >
::SetRhoEnd(float value)
{
    this->m_rhoEnd = value;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
float
Optimus< TFixedImage, TMovingImage, TTransformScalarType >
::GetScalingCoefficient() const
{
    return this->m_scalingCoefficient;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
Optimus< TFixedImage, TMovingImage, TTransformScalarType >
::SetScalingCoefficient(float value)
{
    this->m_scalingCoefficient = value;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
Optimus< TFixedImage, TMovingImage, TTransformScalarType >
::StartRegistration(void)
{


    // Check if fixed image has been set
    if (this->m_fixedImage.IsNull())
        throw std::runtime_error( "Fixed image has not been set." );


    // Check if moving image has been set
    if (this->m_movingImage.IsNull())
        throw std::runtime_error( "Moving image has not been set." );


    // Check if the number of iterations is enough
    if (this->m_iterations<this->m_interpolations)
        throw std::runtime_error( "Maximum number of iterations must be greater than or equal to the number of interpolations." );


    // Modify the registration status
    this->SetRegistrationStatus(RegistrationMethod<TFixedImage, TMovingImage, TTransformScalarType>::REGISTRATION_STATUS_PROCESSING);


    // Type definition
    typedef itk::NewUoaOptimizer                                                        OptimizerType;
    typedef itk::MattesMutualInformationImageToImageMetric< TFixedImage, TMovingImage > MetricType ;
    typedef itk::LinearInterpolateImageFunction< TMovingImage, TTransformScalarType >   InterpolatorType;
    typedef itk::ImageRegistrationMethod< TFixedImage, TMovingImage >                   RegistrationType ;


    // Create the metric, the optimizer, the interpolator, and the registration objects
    typename MetricType::Pointer       metric       = MetricType::New();
    typename OptimizerType::Pointer    optimizer    = OptimizerType::New();
    typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
    typename RegistrationType::Pointer registration = RegistrationType::New();


    // Initialize the registration method
    registration->SetMetric(           metric );
    registration->SetOptimizer(        optimizer );
    registration->SetInterpolator(     interpolator );
    registration->SetFixedImage(       this->m_fixedImage );
    registration->SetMovingImage(      this->m_movingImage );
    registration->SetFixedImageRegion( this->m_fixedImage->GetBufferedRegion() );


    // Initialize the transformation
    typename TransformType::Pointer transform = TransformType::New();
    if (this->m_initialTransform.IsNull())
    {
        typedef itk::CenteredTransformInitializer< TransformType, TFixedImage, TMovingImage >  TransformInitializerType;
        typename TransformInitializerType::Pointer initializer = TransformInitializerType::New();
        initializer->SetTransform(   transform );
        initializer->SetFixedImage(  this->m_fixedImage );
        initializer->SetMovingImage( this->m_movingImage );
        initializer->GeometryOn(); // It's either GeometryOn (center of image) or MomentsOn (center of mass)
        initializer->InitializeTransform();
    }
    else
    {
        transform->SetParameters( this->m_initialTransform->GetParameters() );
        transform->SetCenter(     this->m_initialTransform->GetCenter() );
    }
    registration->SetTransform(                  transform );
    registration->SetInitialTransformParameters( transform->GetParameters() );


    // Initialize the metric
    metric->SetNumberOfHistogramBins(  this->m_histogramBins );
    metric->SetNumberOfSpatialSamples( this->m_spatialSamples );


    // Initialize the optimizer
    optimizer->SetRhoBeg(           this->m_rhoStart );
    optimizer->SetRhoEnd(           this->m_rhoEnd );
    optimizer->SetNbInterp(         this->m_interpolations );
    optimizer->SetScaleTranslation( this->m_scalingCoefficient );
    optimizer->SetMaxFunctionCalls( this->m_iterations );


    // Start the registration process
    try
    {
        registration->UpdateLargestPossibleRegion();
    }
    catch( itk::ExceptionObject & err )
    {
        std::string message = "Unexpected error: ";
        message += err.GetDescription();
        throw std::runtime_error( message  );
    }

    // Set the transformation parameters
    static_cast< TransformType * >(this->m_transform.GetPointer())->SetCenter(     transform->GetCenter() );
    static_cast< TransformType * >(this->m_transform.GetPointer())->SetParameters( registration->GetLastTransformParameters() );


    // Modify the registration status
    this->SetRegistrationStatus(RegistrationMethod<TFixedImage, TMovingImage, TTransformScalarType>::REGISTRATION_STATUS_STOP);
}


} // End of namespace
