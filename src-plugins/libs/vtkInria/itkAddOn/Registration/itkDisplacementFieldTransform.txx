/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "itkDisplacementFieldTransform.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkImageRegionIterator.h"
#include "vnl/vnl_det.h"


namespace itk
{


  /*
 * Constructor
 */

  
// Constructor with default arguments
template <class TScalarType, unsigned int NDimensions>
DisplacementFieldTransform<TScalarType, NDimensions>
::DisplacementFieldTransform():
  Superclass( SpaceDimension, ParametersDimension )
{
  m_DisplacementField = 0;
  m_InterpolateFunction = InterpolateFunctionType::New();
}

// Print self
template<class TScalarType, unsigned int NDimensions>
void
DisplacementFieldTransform<TScalarType, NDimensions>::
PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);  
}


template<class TScalarType, unsigned int NDimensions>
void
DisplacementFieldTransform<TScalarType, NDimensions>::
SetDisplacementField (DisplacementFieldConstPointerType field)
{
  m_DisplacementField = field;
  m_InterpolateFunction->SetInputImage (m_DisplacementField);

  itk::Vector<double, NDimensions> spacing = m_DisplacementField->GetSpacing();
  for (unsigned int i=0; i<NDimensions; i++)
  {
    m_DerivativeWeights[i] = (double)(1.0/spacing[i]);
  }
  
  this->Modified();
}


// Transform a point
template<class TScalarType, unsigned int NDimensions>
typename DisplacementFieldTransform<TScalarType, NDimensions>::OutputPointType
DisplacementFieldTransform<TScalarType, NDimensions>::
TransformPoint(const InputPointType &point) const 
{

  OutputPointType output = point;

  if (m_DisplacementField)
  {
    typename InterpolateFunctionType::OutputType vector = m_InterpolateFunction->Evaluate (output);
    for (unsigned int i=0; i<NDimensions; i++)
    {
      output[i] += vector[i];
    }
    
  }

  
  
  return output;
}


// Transform a vector
template<class TScalarType, unsigned int NDimensions>
typename DisplacementFieldTransform<TScalarType, NDimensions>::OutputVectorType
DisplacementFieldTransform<TScalarType, NDimensions>::
TransformVector(const InputVectorType &vect) const 
{
  itkExceptionMacro ("cannot transform vector !");  
}


// Transform a vnl_vector_fixed
template<class TScalarType, unsigned int NDimensions>
typename DisplacementFieldTransform<TScalarType, NDimensions>::OutputVnlVectorType
DisplacementFieldTransform<TScalarType, NDimensions>::
TransformVector(const InputVnlVectorType &vect) const 
{
  itkExceptionMacro ("cannot transform vnl_vector !");  
}


// Transform a CovariantVector
template<class TScalarType, unsigned int NDimensions>
typename DisplacementFieldTransform<TScalarType, NDimensions>::OutputCovariantVectorType
DisplacementFieldTransform<TScalarType, NDimensions>::
TransformCovariantVector(const InputCovariantVectorType &vect) const 
{  
  itkExceptionMacro ("cannot transform covariant vector !");
}
  
// Compute the Jacobian determinant in one position 
template<class TScalarType, unsigned int NDimensions>
typename DisplacementFieldTransform<TScalarType, NDimensions>::ScalarType
DisplacementFieldTransform< TScalarType,  NDimensions >::
GetJacobianDeterminantWithRespectToCoordinates( const InputPointType & point) const
{
  return static_cast<ScalarType>(vnl_det(this->GetJacobianWithRespectToCoordinates (point)));
}

// Compute the Jacobian in one position 
template<class TScalarType, unsigned int NDimensions>
vnl_matrix_fixed<double,NDimensions,NDimensions>
DisplacementFieldTransform<TScalarType, NDimensions>::
GetJacobianWithRespectToCoordinates( const InputPointType & point) const
{
  
  unsigned int i, j;
  vnl_matrix_fixed<double,NDimensions,NDimensions> J;

  if (!m_DisplacementField)
  {
    itkExceptionMacro ("No displacement field, cannot compute jacobian !");
  }
  
  double weight;
  itk::Vector<double, NDimensions> spacing = m_DisplacementField->GetSpacing();
  
  InputPointType point_prev, point_next;
  
  for (i = 0; i < NDimensions; ++i)
  {
    point_prev[i] = static_cast<double>(point[i]) - spacing[i];
    point_next[i] = static_cast<double>(point[i]) + spacing[i];
  }
  
  typename InterpolateFunctionType::OutputType vector = m_InterpolateFunction->Evaluate (point);
  typename InterpolateFunctionType::OutputType vector_prev = m_InterpolateFunction->Evaluate (point_prev);
  typename InterpolateFunctionType::OutputType vector_next = m_InterpolateFunction->Evaluate (point_next);
  
  for (i = 0; i < NDimensions; ++i)
  {        
    weight = 0.5*m_DerivativeWeights[i];
    
    for (j = 0; j < NDimensions; ++j)
    {
      
      J[i][j]=weight*(static_cast<double>(vector_next[j])
		      -static_cast<double>(vector_prev[j]));
    }
    
    // add one on the diagonal to consider the warp
    // and not only the deformation field
    J[i][i] += 1.0;
  }  
  
  return J;

}


  
} // namespace


