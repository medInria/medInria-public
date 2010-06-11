/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: itkAffineRegistrationMethod.h 1 2008-01-22 19:01:33Z ntoussaint $
Language:  C++
Author:    $Author: pfillard $
Date:      $Date: 2008-01-22 20:01:33 +0100 (Tue, 22 Jan 2008) $
Version:   $Revision: 1 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itk_CenterImageRegistrationMethod_h_
#define _itk_CenterImageRegistrationMethod_h_

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


#endif

