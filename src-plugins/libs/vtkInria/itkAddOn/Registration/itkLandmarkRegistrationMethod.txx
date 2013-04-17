/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "itkLandmarkRegistrationMethod.h"

namespace itk
{

/*
 * Constructor
 */
template <typename TImage>
LandmarkRegistrationMethod<TImage>
::LandmarkRegistrationMethod()
{

  m_Rigid2DInitializer = LandmarkRigid2DTransformInitializerType::New();
  m_Rigid3DInitializer = LandmarkRigid3DTransformInitializerType::New();
  m_NonLinearKernelTransform = NonLinearKernelTransformType::New();

  m_FixedPointSet = PointSetType::New();
  m_MovingPointSet = PointSetType::New();

  this->SetExportTransformType (TRANSFORM_RIGID);

  this->SetName ("Manual Landmark based");
}

  
/*
 * Constructor
 */
template <typename TImage>
void
LandmarkRegistrationMethod<TImage>
::Initialize()
{
  this->Superclass::Initialize ();
}

  

/*
 * PrintSelf
 */
template <typename TImage>
void
LandmarkRegistrationMethod<TImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf( os, indent );
}




template <typename TImage>
void
LandmarkRegistrationMethod<TImage>
::InsertLandmarkPair (PointType FixedImagePoint, PointType MovingImagePoint)
{
  unsigned long id = m_FixedLandmarks.size();
  
  m_FixedLandmarks.push_back (FixedImagePoint);
  m_MovingLandmarks.push_back (MovingImagePoint);

  m_FixedPointSet->GetPoints()->InsertElement (id, FixedImagePoint);
  m_MovingPointSet->GetPoints()->InsertElement (id, MovingImagePoint);

  this->Modified();
  
}
  
  
/*
 * Generate Data
 */
template <typename TImage>
void
LandmarkRegistrationMethod<TImage>
::GenerateData()
{

  typename Rigid2DTransformType::Pointer Transform2D = Rigid2DTransformType::New();
  typename Rigid3DTransformType::Pointer Transform3D = Rigid3DTransformType::New();

  this->UpdateProgress (0);
  
  
  switch (this->GetExportTransformType())
  {
      case TRANSFORM_AFFINE:
	this->UpdateProgress (1);
	this->SetProgress(0.0);
	itkExceptionMacro ("AFFINE TRANSFORMATION NOT SUPPORTED ! !");
	break;

      case TRANSFORM_RIGID:

	switch(ImageType::ImageDimension)
	{
	    case 2:	      

  
	      if (!m_FixedLandmarks.size())
	      {
		this->UpdateProgress (1);
		this->SetProgress(0.0);
		itkWarningMacro ("No landmark to initialize transform.");
	      }
	      else
	      {
		Transform2D->SetIdentity();
		m_Rigid2DInitializer->SetTransform (Transform2D);
		m_Rigid2DInitializer->SetFixedLandmarks (m_FixedLandmarks);
		m_Rigid2DInitializer->SetMovingLandmarks (m_MovingLandmarks);
		this->UpdateProgress (0.5);
		m_Rigid2DInitializer->InitializeTransform();
	      }
	      this->SetTransform ((TransformType*)(Transform2D.GetPointer()));  
	      break;

	    case 3:
	      
	      if (!m_FixedLandmarks.size())
	      {
		this->UpdateProgress (1);
		this->SetProgress(0.0);
		itkWarningMacro ("No landmark to initialize transform.");
	      }
	      else
	      {
		Transform3D->SetIdentity();
		m_Rigid3DInitializer->SetTransform (Transform3D);
		m_Rigid3DInitializer->SetFixedLandmarks (m_FixedLandmarks);
		m_Rigid3DInitializer->SetMovingLandmarks (m_MovingLandmarks);
		this->UpdateProgress (0.5);
		m_Rigid3DInitializer->InitializeTransform();
	      }
	      this->SetTransform ((TransformType*)(Transform3D.GetPointer()));
	      break;
	      
	    default:
	      this->UpdateProgress (1);
	      this->SetProgress(0.0);
	      itkExceptionMacro ("Dimension not supported by LandmarkRegistrationMethod !");
	      break;
	      
	}

	break;
      case TRANSFORM_NON_LINEAR_KERNEL:

	if (!m_FixedLandmarks.size())
	{
	  this->UpdateProgress (1);
	  this->SetProgress(0.0);
	  itkWarningMacro ("No landmark to initialize transform.");
	}
	else
	{  
	  m_NonLinearKernelTransform->SetSourceLandmarks(m_FixedPointSet);
	  m_NonLinearKernelTransform->SetTargetLandmarks(m_MovingPointSet);
	  this->UpdateProgress (0.5);
	  m_NonLinearKernelTransform->ComputeWMatrix();
	}

	this->SetTransform ((TransformType*)(m_NonLinearKernelTransform.GetPointer())); 
	break;

	
      default:
	this->UpdateProgress (1);
	this->SetProgress(0.0);
	itkExceptionMacro ("Transformation type not supported by the LandmarkRegistrationMethod !");
	break;
  }


  this->UpdateProgress (1);
  

}



} // end namespace itk



