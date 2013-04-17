/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "itkRegistrationFactory.h"

#include "itkResampleImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkWindowedSincInterpolateImageFunction.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkInterpolateImageFunction.h"

namespace itk
{



  /** \class ImageRegistrationFactory (itk)
   *   \brief Factory for Image Registration.
   
   * \ingroup   ProcessObject
   *
   * \sa RegistrationFactory
   *
   * \author Nicolas Toussaint, INRIA
   */

  template <typename TImage>
  class ITK_EXPORT ImageRegistrationFactory : public RegistrationFactory < TImage >
  {

  public:


    typedef ImageRegistrationFactory  Self;
    typedef RegistrationFactory < TImage > Superclass;
    typedef SmartPointer<Self>   Pointer;
    typedef SmartPointer<const Self>  ConstPointer;

    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(ImageRegistrationFactory, RegistrationFactory);

    /**  Type of the Fixed image. */
    typedef typename Superclass::ImageType ImageType;
    typedef typename Superclass::ImagePointer ImagePointer;
    typedef typename Superclass::ImageConstPointer ImageConstPointer;
    typedef typename Superclass::CallbackType CallbackType;
    typedef typename Superclass::MethodType MethodType;
    typedef typename Superclass::MethodPointerType MethodPointerType;
    typedef typename Superclass::ParametersValueType ParametersValueType;
    typedef typename Superclass::GeneralTransformType GeneralTransformType;
    typedef typename Superclass::GeneralTransformPointerType GeneralTransformPointerType;
    typedef typename Superclass::TransformType TransformType;
    typedef typename Superclass::TransformPointerType TransformPointerType;
    typedef typename Superclass::LinearTransformType LinearTransformType;
    typedef typename Superclass::DisplacementFieldType DisplacementFieldType;
    typedef typename Superclass::ImageReaderType ImageReaderType;
    typedef typename Superclass::ImageWriterType ImageWriterType;
    typedef typename Superclass::DisplacementFieldReaderType DisplacementFieldReaderType;
    typedef typename Superclass::WarpedImageType WarpedImageType;
    typedef typename Superclass::MethodListType MethodListType;
    
      
    typedef itk::ResampleImageFilter<ImageType, ImageType, ParametersValueType> ResampleImageFilterType;
    typedef typename ResampleImageFilterType::Pointer ResampleImageFilterPointerType;

    typedef InterpolateImageFunction<ImageType, ParametersValueType>
      InterpolateFunctionType;
    typedef LinearInterpolateImageFunction<ImageType, ParametersValueType>
      LinearInterpolateFunctionType;
    typedef NearestNeighborInterpolateImageFunction<ImageType, ParametersValueType>
      NeighborInterpolateFunctionType;
    typedef WindowedSincInterpolateImageFunction<ImageType, ImageType::ImageDimension, Function::HammingWindowFunction<3>, ConstantBoundaryCondition<ImageType>, ParametersValueType>
      SinCardInterpolateFunctionType;
    typedef BSplineInterpolateImageFunction<ImageType, ParametersValueType>
      BSplineInterpolateFunctionType;

    /**
       unsigned int to set the interpolation mode
    */
    enum InterpolationStyleId
    {
      TRILINEAR,
      BSPLINE,
      NEIGHBOR,
      SINCARD
    };

    /** Get/Set Methods for the interpolation mode
	Used to interpolate the registrated image on
	the reference image size and spacing */
    void SetInterpolationStyle (unsigned int style)
    { m_InterpolationStyle = style; }
    unsigned int GetInterpolationStyle() const
    { return m_InterpolationStyle; }

    
  protected:
    ImageRegistrationFactory();
    virtual ~ImageRegistrationFactory() {};

    /** Method invoked by the pipeline in order to trigger the computation of
     * the registration. */
    virtual void  GenerateData ();
    
    unsigned int  m_InterpolationStyle;
    
  private:
    ImageRegistrationFactory(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

  };


} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkImageRegistrationFactory.txx"
#endif


