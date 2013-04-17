/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "itkRegistrationMethod.h"

#include "itkLandmarkBasedTransformInitializer.h"
#include "itkAffineTransform.h"
#include "itkTranslationTransform.h"
#include "itkRigid3DTransform.h"
#include "itkRigid2DTransform.h"
#include "itkVersorRigid3DTransform.h"
#include "itkThinPlateSplineKernelTransform.h"

namespace itk
{

/**
   \class LandmarkRegistrationMethod
   \brief concrete class for landmark based transformation registration
   
   This Class define the interface for landmark based transformation registration.
   That is, that the user might use InsertLandmarkPair() in order to set the
   inputs, which are point correspondances between FixedImage space and MovingImage
   space. Removing landmarks can be done by RemoveAllLandmarks() method.

   Output transformation can be accessed by GetTransform() method.

   ======================================================================
   
   There are two different type of output transformations :


   1. Rigid transformation
   
   Given the input set of landmark pairs, the quaternion based LandmarkBasedTransformInitializer
   is used to find the best rigid transformation matching point pairs. The output
   transformation is of type MatrixOffSetTransformBase (rigid 2D or 3D).

   
   2. Non-Rigid transformation

   Given the input set of landmark pairs, we use here the ThinPlateSplineKernelTransform,
   which uses thin plates splines to match the poit pairs. The output transformation
   is of type ThinPlateSplineKernelTransform, and can be used to non-rigidally deform 
   the image.
   
   For the moment there not much parameters to set to this method but this
   is still work in progress.

   Affine transformation based on landmarks is not yet implemented
   
   \ingroup RegistrationFilters
   \author Nicolas Toussaint, INRIA

*/
template <typename TImage>
class ITK_EXPORT LandmarkRegistrationMethod : public RegistrationMethod <TImage>
{
public:
  /** Standard class typedefs. */
  typedef LandmarkRegistrationMethod  Self;
  typedef RegistrationMethod<TImage>  Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  /** Run-time type information (and related methods). */
  itkTypeMacro(LandmarkRegistrationMethod, RegistrationMethod);
  /** Image type, used for instanciating the registration filters */  
  typedef TImage ImageType;
  /** Superclass typedef copies */
  typedef typename Superclass::ParametersValueType ParametersValueType;
  typedef typename Superclass::TransformType TransformType;
  /** typedef defining one of the possible output transform types : rigid transform (2D case) */
  typedef itk::Rigid2DTransform<ParametersValueType> Rigid2DTransformType;
  typedef typename Rigid2DTransformType::Pointer Rigid2DTransformPointerType;
  /** typedef defining one of the possible output transform types : rigid transform (3D case) */
  typedef itk::VersorRigid3DTransform<ParametersValueType> Rigid3DTransformType;
  typedef typename Rigid3DTransformType::Pointer Rigid3DTransformPointerType;
  /** typedef defining one of the possible output transform types : non-linear kernel-based transfors */
  typedef itk::ThinPlateSplineKernelTransform<ParametersValueType, ImageType::ImageDimension> NonLinearKernelTransformType;
  typedef typename NonLinearKernelTransformType::Pointer NonLinearKernelTransformPointerType;
  /** typedef defining an affine transform : internal use only */
  typedef itk::AffineTransform<ParametersValueType, ImageType::ImageDimension> AffineTransformType;
  /** transform initializer from landmark set : affine case, not supported */
  typedef itk::LandmarkBasedTransformInitializer<AffineTransformType, ImageType, ImageType> LandmarkAffineTransformInitializerType;
  typedef typename LandmarkAffineTransformInitializerType::Pointer LandmarkAffineTransformInitializerPointerType;
  /** transform initializer from landmark set : ridid 2D case */
  typedef itk::LandmarkBasedTransformInitializer<Rigid2DTransformType, ImageType, ImageType> LandmarkRigid2DTransformInitializerType;
  typedef typename LandmarkRigid2DTransformInitializerType::Pointer LandmarkRigid2DTransformInitializerPointerType;
  /** transform initializer from landmark set : ridid 3D case */
  typedef itk::LandmarkBasedTransformInitializer<Rigid3DTransformType, ImageType, ImageType> LandmarkRigid3DTransformInitializerType;
  typedef typename LandmarkRigid3DTransformInitializerType::Pointer LandmarkRigid3DTransformInitializerPointerType;
  /** usefull typedefs derived from the kernel based transforms */
  typedef typename NonLinearKernelTransformType::PointSetType PointSetType;
  typedef typename PointSetType::Pointer PointSetPointerType;
  /** other usefull typedefs */
  typedef typename TransformType::InputPointType PointType;
  typedef typename LandmarkAffineTransformInitializerType::LandmarkPointContainer PointContainer;
  /** enum for the output transform types. affine not supported */
  enum ExportTransformType
  {
    TRANSFORM_RIGID = 0,
    TRANSFORM_AFFINE,
    TRANSFORM_NON_LINEAR_KERNEL
  };
  /**
     Get/Set the output transform type.

     The user can choose between supported types : TRANSFORM_RIGID and TRANSFORM_NON_LINEAR_KERNEL,
     the affine type is not supported by the itk  landmark based transform initializer.
  */
  void SetExportTransformType (unsigned int type)
  { m_ExportTransformType = type; }
  /**
     Get/Set the output transform type.

     The user can choose between supported types : TRANSFORM_RIGID and TRANSFORM_NON_LINEAR_KERNEL,
     the affine type is not supported by the itk  landmark based transform initializer.
  */
  unsigned int GetExportTransformType()
  { return m_ExportTransformType; }
  /**
     insert a pair of landmark into the transform initializers
     This method does not update the output transform dynamically, use Update() method to
     take changes into account
  */
  virtual void InsertLandmarkPair (PointType FixedImagePoint, PointType MovingImagePoint);
  /**
     Removes all previously added landmarks from the initializers
  */
  virtual void RemoveAllLandmarks()
  {
    m_FixedLandmarks.clear();
    m_MovingLandmarks.clear();
    m_FixedPointSet->Initialize();
    m_MovingPointSet->Initialize();
    this->Modified();
  }
  /** Initialize by setting the interconnects between the components. */
  virtual void Initialize();
  
protected:
  LandmarkRegistrationMethod();
  virtual ~LandmarkRegistrationMethod() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Method invoked by the pipeline in order to trigger the computation of 
   * the registration. */
  virtual void  GenerateData ();

  LandmarkAffineTransformInitializerPointerType  m_AffineInitializer;
  LandmarkRigid2DTransformInitializerPointerType m_Rigid2DInitializer;
  LandmarkRigid3DTransformInitializerPointerType m_Rigid3DInitializer;
  NonLinearKernelTransformPointerType            m_NonLinearKernelTransform;
  
  unsigned int m_ExportTransformType;
  PointContainer m_FixedLandmarks;
  PointContainer m_MovingLandmarks;
  PointSetPointerType m_FixedPointSet;
  PointSetPointerType m_MovingPointSet;
  

    
private:
  LandmarkRegistrationMethod(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  

};


} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLandmarkRegistrationMethod.txx"
#endif
  



