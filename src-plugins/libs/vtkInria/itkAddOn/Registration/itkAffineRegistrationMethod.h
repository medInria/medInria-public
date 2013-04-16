/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "itkRegistrationMethod.h"

#include "itkAffineTransform.h"
#include "itkMultiResolutionImageRegistrationMethod.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkRecursiveMultiResolutionPyramidImageFilter.h"


namespace itk
{

/**
   \class AffineRegistrationInterfaceCommand
   \brief Just a little class to handle iteration event observations
   
   \ingroup Command
   \author Nicolas Toussaint, INRIA
   
*/
template <typename TImageType>
class AffineRegistrationInterfaceCommand : public itk::Command 
{  
  
 public:
  
  typedef AffineRegistrationInterfaceCommand   Self;
  typedef itk::Command                         Superclass;
  typedef itk::SmartPointer<Self>              Pointer;
  itkNewMacro( Self );
  
  typedef TImageType ImageType;
  
  typedef itk::MultiResolutionImageRegistrationMethod<ImageType, ImageType> RegistrationType;
  typedef typename RegistrationType::Pointer RegistrationPointer;
  
  typedef itk::RegularStepGradientDescentOptimizer OptimizerType;
  typedef typename OptimizerType::Pointer OptimizerPointer;
  
  typedef itk::ProcessObject ProcessObjectType;
  typedef typename ProcessObjectType::Pointer ProcessObjectPointer;

  void SetNumberOfIterations (std::vector<unsigned int> table)
  {
    m_MaxIterations = 0;
    m_IterationsTable = table;
    m_CurrentLevel = 0;
    
    
    unsigned int D = ImageType::ImageDimension;
    
    for (unsigned int i=0; i<table.size(); i++)
    {
      m_MaxIterations += (unsigned int)( table[i] * std::pow ((float)2.0,(int)(i*D)) );
    }
    m_LevelWeights.clear();
    for (unsigned int i=0; i<table.size(); i++)
    {
      m_LevelWeights.push_back ((double)( table[i] * std::pow ((float)2.0,(int)(i*D)) )/(double)(m_MaxIterations));
    }
    
  }
  
  void SetItkObjectToWatch (ProcessObjectPointer object)
  {
    m_ProcessObject = object;
  }
  
  void Execute(itk::Object * object, const itk::EventObject & event)
  {
    if( !(itk::IterationEvent().CheckEvent( &event )) )
    {
      return;
    }
    
    OptimizerPointer optimizer = dynamic_cast< OptimizerType* >(object );
    RegistrationPointer registration = dynamic_cast< RegistrationType* >(object );
    
    if (!optimizer && !registration)
      return;

    if (optimizer)
    {
      
    double progress = (double)optimizer->GetCurrentIteration() + 1.0;
    if (m_IterationsTable.size())
    {
      progress += (double)(m_CurrentLevel * optimizer->GetNumberOfIterations());
      progress /= (double)(m_IterationsTable.size() * optimizer->GetNumberOfIterations());
    }
    
/*       double progress = optimizer->GetCurrentIteration() + 1;      */
/*       if (m_IterationsTable.size()) */
/*       { */
/* 	progress /= (double)m_IterationsTable[m_CurrentLevel]; */
/* 	progress *= m_LevelWeights[m_CurrentLevel]; */
/* 	for (unsigned int i=0; i<m_CurrentLevel; i++) */
/* 	  progress += m_LevelWeights[m_CurrentLevel-1]; */
/*       } */
      
      if( m_ProcessObject )
      {
	m_ProcessObject->UpdateProgress( progress );
      }
    }
    else
    {
      m_CurrentLevel = registration->GetCurrentLevel();
      OptimizerPointer registrationoptimizer = dynamic_cast< OptimizerType* >(registration->GetOptimizer() );
      if (registrationoptimizer && (m_IterationsTable.size() > m_CurrentLevel) )
      {
	registrationoptimizer->SetNumberOfIterations (m_IterationsTable[m_CurrentLevel]);
      }
    }
    
  }
  
  void Execute(const itk::Object * caller, const itk::EventObject & event)
  {
    
    Execute( const_cast<itk::Object *>(caller), event);
    return;
  }
  
  
 protected:
  AffineRegistrationInterfaceCommand()
  {
    m_ProcessObject=NULL;
    m_CurrentLevel=0;
    m_MaxIterations = 100;
    
  }
  
  unsigned int m_CurrentLevel;
  unsigned int m_MaxIterations;
  std::vector<double> m_LevelWeights;
  std::vector<unsigned int> m_IterationsTable;
  ProcessObjectPointer m_ProcessObject;
  
};





  
/**
   \class AffineRegistrationMethod
   \brief concrete class for Affine Registration 
   
   This Class define the interface for affine transformation registration.
   
   For the moment there not much parameters to set to this method but this
   is still work in progress.
   
   The method uses a multi-resolution registration process to compute the
   transform. A Mattes mutual information metric is used for minimization.
   The Optimization is a regular gradient descent process, and a linear
   interpolation between pixels is used.
   
   Output transform is an affine tranform (AffineTransformType).

   In a near future the user will be able to set parameters such as the
   number of levels (steps) of the multi-resolution registration process,
   the interpolation style, the metric type, etc.
   
   \ingroup RegistrationFilters
   \author Nicolas Toussaint, INRIA

*/
template <typename TImage>
class ITK_EXPORT AffineRegistrationMethod : public RegistrationMethod <TImage>
{
public:
  /** Standard class typedefs. */
  typedef AffineRegistrationMethod  Self;
  typedef RegistrationMethod<TImage>  Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  /** Run-time type information (and related methods). */
  itkTypeMacro(AffineRegistrationMethod, RegistrationMethod);
  /** Image type, used for instanciating the registration filters */
  typedef TImage ImageType;
  /** Superclass typedef copies */
  typedef typename Superclass::ParametersValueType ParametersValueType;
  typedef typename Superclass::TransformType TransformType;
  /** image pixel type */
  typedef typename ImageType::PixelType PixelType;
  /** we force here to use a regular stepgradient optimization scheme */
  typedef itk::RegularStepGradientDescentOptimizer OptimizerType;
  typedef typename OptimizerType::Pointer OptimizerPointerType;
  /** typedef defining scale the optimization weights between transform parameters */
  typedef typename OptimizerType::ScalesType OptimizerScalesType;
  /** general image-to-image metric typedef */
  typedef itk::ImageToImageMetric<ImageType, ImageType> MetricType;    
  typedef typename MetricType::Pointer MetricPointerType;
  /** default metric is a mutual information metric */
  typedef itk::MattesMutualInformationImageToImageMetric<ImageType, ImageType> MutualInformationMetricType;    
  typedef typename MutualInformationMetricType::Pointer MutualInformationMetricPointerType;
  /** General Interpolation scheme typedef */
  typedef typename MetricType::InterpolatorType InterpolatorType;
  typedef typename InterpolatorType::Pointer InterpolatorPointerType;
  /** default interpolation function is a regular linear one */
  typedef itk::LinearInterpolateImageFunction<ImageType, ParametersValueType> LinearInterpolatorType;
  typedef typename LinearInterpolatorType::Pointer LinearInterpolatorPointerType;
  /** we force here to use a multi-resolution scheme as the registration method */
  typedef itk::MultiResolutionImageRegistrationMethod<ImageType, ImageType> RegistrationType;
  typedef typename RegistrationType::Pointer RegistrationPointerType;
  /** typedef defining the output transform type as a affine matrix transform */
  typedef itk::AffineTransform<ParametersValueType, ImageType::ImageDimension> AffineTransformType;
  /** typedef for the iteration callback to handle progress events */
  typedef itk::AffineRegistrationInterfaceCommand<ImageType> CommandType;
  typedef typename CommandType::Pointer CommandPointerType;
  /** we use pyramids to better describe images in the multi-resolution registration method */
  typedef itk::RecursiveMultiResolutionPyramidImageFilter<ImageType, ImageType> ImagePyramidType;
  typedef typename ImagePyramidType::Pointer ImagePyramidPointerType;

  typedef typename Superclass::ScheduleType ScheduleType;

  /** A structure to store parameters of the registration process */
  struct AffineParameters
  {
    MetricPointerType Metric;
    InterpolatorPointerType Interpolator;
    double OptimizationScale;
    unsigned int NumberOfLevels;
    unsigned int* NumberOfIterationList;
  };

  /**
     Set a registration metric to be used
     in the optimization process.

     The default metric is a mutual information metric
  */
  void SetRegistrationMetric (MetricPointerType metric);
  /**
    Set a registration interpolation function. If not set, the registration process
    uses a linear interpolation by default.
  */
  void SetRegistrationInterpolator (InterpolatorPointerType interpolator);
  /**
    Get/Set the number of levels used in the multi-resolution registration process.

    By default (if not set) the method uses 3 different levels
  */
  void SetRegistrationNumberOfLevels (unsigned int n);
  unsigned int GetRegistrationNumberOfLevels() const;
  /**
     Get/Set the maximum number of iterations used in each level of the registration process.

     By default this list is set to {50,25,10}.
  */
  void SetRegistrationNumberOfIterationsList (unsigned int* n_list);
  const unsigned int* GetRegistrationNumberOfIterationsList() const;
  /**
     Set the translation scaling.

     As we process affine registration we try to optimize both the "rotation"
     and "translation" parameters of an affine matrix, these parameters don't describe\
     the same movement and hence have different "range". The optimization process allows
     to weight these different kind of parameters by this scaling value.

     In practice the translation parameters move faster than the rotation ones.
     Then we propose here to define a translation scaling.

     By default this scale is 1.0/1'000.
  */
  void SetTranslationScale (double scale);
  double GetTranslationScale() const;

  
  /** Initialize by setting the interconnects between the components. */
  virtual void Initialize();

    /**
     Default is false.
  */
  itkSetMacro (AutoPyramidSchedule, bool);
  itkGetConstReferenceMacro (AutoPyramidSchedule, bool);
  itkBooleanMacro (AutoPyramidSchedule);

  /** Get the multi-resolution schedule. */
  itkGetConstReferenceMacro(Schedule, ScheduleType);
  void SetSchedule (ScheduleType schedule);


protected:
  AffineRegistrationMethod();
  virtual ~AffineRegistrationMethod() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Method invoked by the pipeline in order to trigger the computation of 
   * the registration. */
  virtual void  GenerateData ();

  AffineParameters Parameters;
  bool m_AutoPyramidSchedule;
  
  ScheduleType m_Schedule;
  
  
private:
  AffineRegistrationMethod(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  

};


} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkAffineRegistrationMethod.txx"
#endif
  



