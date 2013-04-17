/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

// This example class implements a non rigid algorithm with a displacement field transform, change for rigid or affine.

#include <itkDisplacementFieldTransform.h>
#include "rpiRegistrationMethod.hxx"


// Namespace RPI : Registration Programming Interface
namespace rpi
{
    
    
    /**
     * %1 registration method. This is just an interface class to your own registration method. It is here to have 
     * a common way of accessing your registration from the registration workspace.
     *
     * There are three templates for this class:
     *
     *   TFixedImage           Type of the fixed image. Must be an itk::Image< TPixel, 3 > where
     *                         TPixel is a scalar type among "float" and "double".
     *
     *   TMovingImage          Type of the moving image. Must be an itk::Image< TPixel, 3 > where
     *                         TPixel is a scalar type among "float" and "double".
     *
     *   TTransformScalarType  Type of the transformation parameters. Must be "float" or "double".
     *
     * In the current implementation, TFixedImage and TMovingImage must be identical.
     *
     */
    template < class TFixedImage, class TMovingImage, class TTransformScalarType=double >
    class ITK_EXPORT %1 : public RegistrationMethod< TFixedImage, TMovingImage, TTransformScalarType >
    {
        
    public:
                
        typedef itk::DisplacementFieldTransform< TTransformScalarType, TFixedImage::ImageDimension >
        TransformType;
        
        typedef typename TransformType::Pointer
        TransformPointerType;
        
        
    protected:
        
        /**
         * Here go the parameters of the method (number of iterations , ...)
         */
        
    public:
        
        /**
         * Class constructor.
         */
        %1();
        
        
        /**
         * Class destructor.
         */
        virtual ~%1();
        
        
        /**
         * Setters and getters go here
         */
        
        
        /**
         * Performs the image registration. Must be called before GetTransformation().
         */
        virtual void StartRegistration();
        
    };
    
} // End of namespace


/** Add the source code file (template) */
#include "rpi%1.hxx"



