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

namespace itk
{


  template <class TImage>
    class NullRegistrationMethod : public RegistrationMethod <TImage>
  {

  public:

    typedef NullRegistrationMethod      Self;
    typedef RegistrationMethod <TImage> Superclass;
    typedef SmartPointer<Self>          Pointer;
    typedef SmartPointer<const Self>    ConstPointer;


    itkNewMacro (Self);
    itkTypeMacro (NullRegistrationMethod, RegistrationMethod);

    typedef typename Superclass::ParametersValueType ParametersValueType;
    typedef typename Superclass::TransformType       TransformType;
    typedef itk::AffineTransform<ParametersValueType, TImage::ImageDimension> AffineTransformType;
    
  protected:
    NullRegistrationMethod()
    {
      this->SetName ("Null");
    }
    ~NullRegistrationMethod() {};

    void PrintSelf(std::ostream& os, Indent indent) const {};
    virtual void  GenerateData ()
    {  
      typename AffineTransformType::Pointer transform = AffineTransformType::New();
      transform->SetIdentity();
      this->SetTransform ( transform );
    }

  private:
    NullRegistrationMethod (const Self&);
    void operator=(const Self&);

    
  };
  


} // end of namespace



