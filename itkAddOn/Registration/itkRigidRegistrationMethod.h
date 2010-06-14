/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: itkRigidRegistrationMethod.h 1 2008-01-22 19:01:33Z fdru, dbarbeau $
Language:  C++
Author:    $Author: fdru, dbarbeau $
Date:      $Date: 2008-01-22 20:01:33 +0100 (Tue, 22 Jan 2008) $
Version:   $Revision: 1 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkRigidRegistrationMethod_h
#define __itkRigidRegistrationMethod_h


#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkImageRegistrationMethod.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkPowellOptimizer.h"
#include "itkEuler3DTransform.h"
#include "itkCenteredTransformInitializer.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

namespace itk
{


template <typename TImage>
class ITK_EXPORT RigidRegistrationMethod : public RegistrationMethod <TImage>
{
public:
  /** Standard class typedefs. */
  typedef RigidRegistrationMethod     Self;
  typedef RegistrationMethod<TImage>  Superclass;
  typedef SmartPointer<Self>          Pointer;
  typedef SmartPointer<const Self>    ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  /** Run-time type information (and related methods). */
  itkTypeMacro(RigidRegistrationMethod, RegistrationMethod);

  typedef TImage                                   ImageType;
  typedef typename Superclass::ParametersValueType ParametersValueType;
  typedef typename ImageType::PixelType            PixelType;

  // Registration types
  typedef itk::Euler3DTransform<double>                                        TransformType;
  typedef itk::PowellOptimizer                                                 OptimizerType;
  typedef itk::MattesMutualInformationImageToImageMetric<ImageType,ImageType > MetricType ;
  typedef itk::LinearInterpolateImageFunction<ImageType,double>                InterpolatorType;
  typedef itk::ImageRegistrationMethod< ImageType, ImageType>                  RegistrationType;
  
  virtual void Initialize(void);

protected:
           RigidRegistrationMethod();
  virtual ~RigidRegistrationMethod() {};
  virtual void PrintSelf(std::ostream& os, Indent indent) const;

  virtual void  GenerateData ();
  
private:
  RigidRegistrationMethod(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  typename MetricType::Pointer                  m_metric;
  typename TransformType::Pointer               m_transform;
  typename OptimizerType::Pointer               m_optimizer;
  typename InterpolatorType::Pointer            m_interpolator;
  typename RegistrationType::Pointer            m_registration;
  

};


} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkRigidRegistrationMethod.txx"
#endif
  
#endif


