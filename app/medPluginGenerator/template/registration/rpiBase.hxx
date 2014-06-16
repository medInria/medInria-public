/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

// Namespace RPI : Registration Programming Interface
namespace rpi
{
    
    template < class TFixedImage, class TMovingImage, class TTransformScalarType >
    %1 < TFixedImage, TMovingImage, TTransformScalarType >
    ::%1()
    {
        // Initialize parameters
        
        // Initialize the transformation
        this->m_transform = TransformType::New();
    }
    
    template < class TFixedImage, class TMovingImage, class TTransformScalarType >
    %1< TFixedImage, TMovingImage, TTransformScalarType >
    ::~%1()
    {
        // Do nothing
    }
        
    template < class TFixedImage, class TMovingImage, class TTransformScalarType >
    void
    %1< TFixedImage, TMovingImage, TTransformScalarType >
    ::StartRegistration()
    {
        
        // Check if fixed image has been set
        if (this->m_fixedImage.IsNull())
            throw std::runtime_error( "Fixed image has not been set." );
        
        
        // Check if moving image has been set
        if (this->m_movingImage.IsNull())
            throw std::runtime_error( "Moving image has not been set." );
        
        
        // Type definition
        typedef  typename  TransformType::ScalarType
        ScalarType;
        
        typedef  typename  TransformType::VectorFieldType
        VectorFieldType;        
        
        // Local images
        typename  TFixedImage::ConstPointer   fixedImage  = this->m_fixedImage;
        typename  TMovingImage::ConstPointer  movingImage = this->m_movingImage;
                        
        // Set the displacement field to the transformation object
        // This will be done from your registration output, using something similar to this for non rigid transforms
        //static_cast< TransformType * >(this->m_transform.GetPointer())->SetParametersAsVectorField(static_cast<typename VectorFieldType::ConstPointer>(multires->GetOutput()));
    }
    
    
} // End of namespace



