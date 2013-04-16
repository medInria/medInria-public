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
#include "itkCenteredTransformInitializer.h"

namespace itk
{
  
  template <class TImage>
    class CenterImageRegistrationMethod : public RegistrationMethod <TImage>
  {

  public:

    typedef CenterImageRegistrationMethod      Self;
    typedef RegistrationMethod <TImage> Superclass;
    typedef SmartPointer<Self>          Pointer;
    typedef SmartPointer<const Self>    ConstPointer;


    itkNewMacro (Self);
    itkTypeMacro (CenterImageRegistrationMethod, RegistrationMethod);

    typedef typename Superclass::ParametersValueType ParametersValueType;
    typedef typename Superclass::TransformType       TransformType;
    typedef itk::AffineTransform<ParametersValueType, TImage::ImageDimension> AffineTransformType;
    
  protected:
    CenterImageRegistrationMethod()
    {
      this->SetName ("Center Images");
    }
    ~CenterImageRegistrationMethod() {};

    void PrintSelf(std::ostream& os, Indent indent) const {};
    virtual void  GenerateData ()
    {  
      typename AffineTransformType::Pointer transform = AffineTransformType::New();
      transform->SetIdentity();
      
      typedef itk::CenteredTransformInitializer<AffineTransformType, TImage, TImage>
	TransformInitializerType;
      
      typename TransformInitializerType::Pointer initializer = TransformInitializerType::New();
      
      initializer->SetTransform(   transform );
      
      initializer->SetFixedImage(  this->GetFixedImage() );
      initializer->SetMovingImage( this->GetMovingImage() );
      
      initializer->MomentsOn();
      
      initializer->InitializeTransform();

      
      this->SetTransform ( transform );
    }

  private:
    CenterImageRegistrationMethod (const Self&);
    void operator=(const Self&);

    
  };
  


} // end of namespace




