/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "itkTransform.h"
#include "itkProcessObject.h"
#include "itkConstNeighborhoodIterator.h"

#include "itkImage.h"

#include "itkVectorLinearInterpolateNearestNeighborExtrapolateImageFunction.h"

namespace itk
{
/**
   \class DisplacementFieldTransform (itk)

   This class is a concrete implementation of Transform that handle Non-Linear
   displacements described by a DisplacementField

   This class is templated into the parameter value type (TScalarType),
   and the dimension of the displacements.

   This transform does not contain any parameters, there would be too many of them ( DimX
   x DimY x DimZ x 3 double scalar parameters for a 3D space example, plus other fixed parameters...)
   
   Use SetDisplacementField() to be able to get the correspondant transform
   This transform is only able to transform points. Transforming a vector and a
   covariant vector has no meaning in the sense that we need to transform
   the vector with the jacobian of the transformation with respect to the parameters,
   which cannot be computed.

   The user can compute the Jacobian of the transformation with respect to the coordinates
   at a specific location with the method GetJacobianWithRespectToCoordinates(), and its determinant (scalar) with
   the method GetJacobianDeterminantWithRespectToCoordinates(). However, the determinant of the
   transformation with respect its parameters can not be computed as there is no parametric
   handling.

   There is no I/O support as there is no parameters, the user might save the
   displacement field instead.
   
   \ingroup RegistrationFilters
   \author Nicolas Toussaint, INRIA
*/

  template <class TScalarType=float, unsigned int NDimensions=3>
    class ITK_EXPORT DisplacementFieldTransform : public Transform<TScalarType, NDimensions, NDimensions>
    {
      
    public:    
    
    typedef DisplacementFieldTransform         Self;
    typedef Transform<TScalarType, NDimensions, NDimensions> Superclass;
    typedef SmartPointer<Self>       Pointer;
    typedef SmartPointer<const Self> ConstPointer;

    /** Dimension of the domain space. */
    itkStaticConstMacro(SpaceDimension, unsigned int, NDimensions);
    itkStaticConstMacro(ParametersDimension, unsigned int, NDimensions);

    /** generic constructors */
    itkNewMacro (Self);
    itkTypeMacro (DisplacementFieldTransform, Transform);

    typedef Superclass TransformType;
    typedef typename TransformType::ConstPointer TransformPointerType;
    
    /** Type of the scalar representing coordinate and vector elements. */
    typedef TScalarType ScalarType;
    
    /** Type of the input parameters. */
    typedef typename Superclass::ParametersType ParametersType;
    
    /** Type of the Jacobian matrix. */
    typedef typename Superclass::JacobianType JacobianType;
    
    /** Standard vector type for this class. */
    typedef typename Superclass::InputVectorType InputVectorType;
    typedef typename Superclass::OutputVectorType OutputVectorType;
    
    /** Standard covariant vector type for this class */
    typedef typename Superclass::InputCovariantVectorType InputCovariantVectorType;
    typedef typename Superclass::OutputCovariantVectorType OutputCovariantVectorType;
  
    /** Standard vnl_vector type for this class. */
    typedef typename Superclass::InputVnlVectorType InputVnlVectorType;
    typedef typename Superclass::OutputVnlVectorType OutputVnlVectorType;
    
    /** Standard coordinate point type for this class */
    typedef typename Superclass::InputPointType InputPointType;
    typedef typename Superclass::OutputPointType OutputPointType;
    
    typedef itk::Vector<ScalarType, NDimensions> VectorType;
    typedef itk::Image<VectorType, NDimensions> DisplacementFieldType;
    typedef typename DisplacementFieldType::Pointer DisplacementFieldPointerType;
    typedef typename DisplacementFieldType::ConstPointer DisplacementFieldConstPointerType;
    typedef typename DisplacementFieldType::IndexType DisplacementFieldIndexType;
    
    typedef itk::VectorLinearInterpolateNearestNeighborExtrapolateImageFunction<DisplacementFieldType, ScalarType> InterpolateFunctionType;
    typedef typename InterpolateFunctionType::Pointer InterpolateFunctionPointerType;

    /**
       Type of the iterator that will be used to move through the image.  Also
	the type which will be passed to the evaluate function
    */
    typedef ConstNeighborhoodIterator<DisplacementFieldType> ConstNeighborhoodIteratorType;
    typedef typename ConstNeighborhoodIteratorType::RadiusType RadiusType;  

    /**  Method to transform a point. */
    virtual OutputPointType TransformPoint(const InputPointType  & ) const;
    /**  Method to transform a vector. */
    virtual OutputVectorType    TransformVector(const InputVectorType &) const;
    /**  Method to transform a vnl_vector. */
    virtual OutputVnlVectorType TransformVector(const InputVnlVectorType &) const;
    /**  Method to transform a CovariantVector. */
    virtual OutputCovariantVectorType TransformCovariantVector(const InputCovariantVectorType &) const;    
    /**
       Set the transformation parameters and update internal transformation.
       * SetParameters gives the transform the option to set it's
       * parameters by keeping a reference to the parameters, or by
       * copying.  To force the transform to copy it's parameters call
       * SetParametersByValue.
       * \sa SetParametersByValue
       * 
       ######## NOT SUPPORTED ############
       */
    virtual void SetParameters( const ParametersType & )
    {
      itkExceptionMacro ("this type of transform does not handle any parameters yet !");
    };
    /**
       Set the transformation parameters and update internal transformation. 
       * This method forces the transform to copy the parameters.  The
       * default implementation is to call SetParameters.  This call must
       * be overridden if the transform normally implements SetParameters
       * by keeping a reference to the parameters.
       * \sa SetParameters

       ######## NOT SUPPORTED ############
       
    */
    virtual void SetParametersByValue ( const ParametersType & p ) 
    {
      itkExceptionMacro ("this type of transform does not handle any parameters yet !");
    };
    
    /**
       Get the Transformation Parameters.
       
       ######## NOT SUPPORTED ############ 
    */
    virtual const ParametersType& GetParameters() const
    {
      itkExceptionMacro ("this type of transform does not handle any parameters yet !");
    }
    

    /**
       Compute the Jacobian of the transformation
       *
       * This method computes the Jacobian matrix of the transformation
       * at a given input point. The rank of the Jacobian will also indicate 
       * if the transform is invertible at this point.
       *
       * The Jacobian is be expressed as a matrix of partial derivatives of the
       * output point components with respect to the parameters that defined
       * the transform:
       *
       * \f[
       *
       J=\left[ \begin{array}{cccc}
       \frac{\partial x_{1}}{\partial p_{1}} & 
       \frac{\partial x_{1}}{\partial p_{2}} & 
       \cdots  & \frac{\partial x_{1}}{\partial p_{m}}\	\
       \frac{\partial x_{2}}{\partial p_{1}} & 
       \frac{\partial x_{2}}{\partial p_{2}} & 
       \cdots  & \frac{\partial x_{2}}{\partial p_{m}}\	\
       \vdots  & \vdots  & \ddots  & \vdots \		\
       \frac{\partial x_{n}}{\partial p_{1}} & 
       \frac{\partial x_{n}}{\partial p_{2}} & 
       \cdots  & \frac{\partial x_{n}}{\partial p_{m}}
       \end{array}\right] 
       *
       * \f]
       *
       ######## NOT SUPPORTED ############ 
       *
       */
    virtual const JacobianType & GetJacobian(const InputPointType  &) const
    {
      itkExceptionMacro ("this type of transform does not handle Jacobian !");
    }
    /**
       Get the jacobian of the transformation with respect to the coordinates at a specific point
    */
    virtual vnl_matrix_fixed<double,NDimensions,NDimensions> GetJacobianWithRespectToCoordinates(const InputPointType  &) const;
    /**
       Get the jacobian determinant of transformation with respect to the coordinates at a specific point
    */
    virtual ScalarType GetJacobianDeterminantWithRespectToCoordinates(const InputPointType  &) const;
    /**
       Get the inverse of this transform

       ######## NOT SUPPORTED ############ 
    */
    virtual bool GetInverse( Self* inverse) const
    {
      itkExceptionMacro ("this type of transform does not handle Inversion !");
      return false;
    }
    
    /**
       Set the transform to identity. Basically remove (and unregister)
       the displacement field from the transform
    */
    virtual void SetIdentity()
    {
      m_DisplacementField = NULL;
    }
    /**
       Set/Get the displacement field used for transforming points and vectors
    */
    itkGetConstObjectMacro( DisplacementField, DisplacementFieldType );
    virtual void SetDisplacementField (DisplacementFieldConstPointerType field);
    
    
    protected:
    
    /** Print contents of an TranslationTransform. */
    void PrintSelf(std::ostream &os, Indent indent) const;

    DisplacementFieldTransform();
    virtual ~DisplacementFieldTransform() {};
    
    DisplacementFieldConstPointerType m_DisplacementField;
    InterpolateFunctionPointerType m_InterpolateFunction;
    
    private:

    /** The weights used to scale partial derivatives during processing */
    double m_DerivativeWeights[NDimensions];
    
    };
  
  
} // end of namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkDisplacementFieldTransform.txx"
#endif
  


