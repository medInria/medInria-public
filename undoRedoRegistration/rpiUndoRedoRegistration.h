#ifndef _RPIUNDOREDOREGISTRATION_H_
#define _RPIUNDOREDOREGISTRATION_H_

// This example class implements a non rigid algorithm with a displacement field transform, change for rigid or affine.

#include <itkDisplacementFieldTransform.h>
#include "rpiRegistrationMethod.hxx"


// Namespace RPI : Registration Programming Interface
namespace rpi
{
    
    
    /**
     * UndoRedoRegistration registration method. This is just an interface class to your own registration method. It is here to have 
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
    class ITK_EXPORT UndoRedoRegistration : public RegistrationMethod< TFixedImage, TMovingImage, TTransformScalarType >
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
        UndoRedoRegistration(void);
        
        
        /**
         * Class destructor.
         */
        virtual ~UndoRedoRegistration(void);
        
        
        /**
         * Setters and getters go here
         */
        
        
        /**
         * Performs the image registration. Must be called before GetTransformation().
         */
        virtual void StartRegistration(void);
        
    };
    
} // End of namespace


/** Add the source code file (template) */
#include "rpiUndoRedoRegistration.hxx"


#endif // _RPIUNDOREDOREGISTRATION_H_
