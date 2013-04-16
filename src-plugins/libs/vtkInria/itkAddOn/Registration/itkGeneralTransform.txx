/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "itkGeneralTransform.h"


namespace itk
{


/*
 * Constructor
 */
// Constructor with default arguments
template <class TScalarType, unsigned int NDimensions>
GeneralTransform<TScalarType, NDimensions>
::GeneralTransform():
  Superclass( SpaceDimension, ParametersDimension )
{
}

/*
 * Destructor
 */
// Constructor with default arguments
template <class TScalarType, unsigned int NDimensions>
GeneralTransform<TScalarType, NDimensions>
::~GeneralTransform()
{
  m_RemovedTransformList.clear();
  m_TransformList.clear();
}


// Set the parameters
template <class TScalarType, unsigned int NDimensions>
void
GeneralTransform<TScalarType, NDimensions>
::SetParameters( const ParametersType & parameters )
{
  itkExceptionMacro("GeneralTransform<TScalarType, NDimensions> does not handle any parameter system yet");
  
}




// Get the parameters
template <class TScalarType, unsigned int NDimensions>
const typename GeneralTransform<TScalarType, NDimensions>::ParametersType &
GeneralTransform<TScalarType, NDimensions>
::GetParameters() const
{
  itkExceptionMacro("GeneralTransform<TScalarType, NDimensions> does not handle any parameter system yet");

  return this->m_Parameters;
}



// Print self
template<class TScalarType, unsigned int NDimensions>
void
GeneralTransform<TScalarType, NDimensions>::
PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "Number Of Transforms in Stack : " << this->GetNumberOfTransformsInStack() << std::endl;
  os << indent << "Is Transform Linear ? : " << this->IsLinear() << std::endl;

}


// Transform a point
template<class TScalarType, unsigned int NDimensions>
typename GeneralTransform<TScalarType, NDimensions>::OutputPointType
GeneralTransform<TScalarType, NDimensions>::
TransformPoint(const InputPointType &point) const
{

  OutputPointType output;
  output = point;

  typename TransformListType::const_reverse_iterator it = m_TransformList.rbegin();
  
  while(it != m_TransformList.rend())
  {
    output = (*it)->TransformPoint (output);
    it++;
  }

  return output;
}


// Transform a vector
template<class TScalarType, unsigned int NDimensions>
typename GeneralTransform<TScalarType, NDimensions>::OutputVectorType
GeneralTransform<TScalarType, NDimensions>::
TransformVector(const InputVectorType &vect) const
{
  OutputVectorType output;
  output = vect;

  typename TransformListType::const_reverse_iterator it = m_TransformList.rbegin();

  while(it != m_TransformList.rend())
  {
    output = (*it)->TransformVector (output);
    it++;
  }

  return  output;
}


// Transform a vnl_vector_fixed
template<class TScalarType, unsigned int NDimensions>
typename GeneralTransform<TScalarType, NDimensions>::OutputVnlVectorType
GeneralTransform<TScalarType, NDimensions>::
TransformVector(const InputVnlVectorType &vect) const
{
  OutputVnlVectorType output;
  output = vect;

  typename TransformListType::const_reverse_iterator it = m_TransformList.rbegin();

  while(it != m_TransformList.rend())
  {
    output = (*it)->TransformVector (output);
    it++;
  }

  return  output;

}

// Transform a CovariantVector
template<class TScalarType, unsigned int NDimensions>
typename GeneralTransform<TScalarType, NDimensions>::OutputCovariantVectorType
GeneralTransform<TScalarType, NDimensions>::
TransformCovariantVector(const InputCovariantVectorType &vect) const
{
  itkExceptionMacro("cannot transform covariant vector with this type of transform");
  return  vect;
}


// return an inverse transformation
template<class TScalarType, unsigned int NDimensions>
bool
GeneralTransform<TScalarType, NDimensions>::
GetInverse(Self*) const
{
  return false;
}


// Set the parameters for an Identity transform of this class
template<class TScalarType, unsigned int NDimensions>
void
GeneralTransform<TScalarType, NDimensions>::
SetIdentity()
{
  this->RemoveAllTransforms();
}



template<class TScalarType, unsigned int NDimensions>
bool
GeneralTransform<TScalarType, NDimensions>::
Redo()
{
   if (!m_RemovedTransformList.size())
      return false;
   m_TransformList.push_back(m_RemovedTransformList.back());
   m_RemovedTransformList.pop_back();
   return true;
}

template<class TScalarType, unsigned int NDimensions>
bool
GeneralTransform<TScalarType, NDimensions>::
Undo()
{
   if (!m_TransformList.size())
      return false;

   m_RemovedTransformList.push_back(m_TransformList.back());
   m_TransformList.pop_back();
   return true;
}

template<class TScalarType, unsigned int NDimensions>
unsigned int
GeneralTransform<TScalarType, NDimensions>::
InsertTransform (TransformConstPointerType arg)
{
   m_TransformList.push_back(arg);
   m_RemovedTransformList.clear();
   return m_TransformList.size();
}



template<class TScalarType, unsigned int NDimensions>
typename GeneralTransform<TScalarType, NDimensions>::TransformConstPointerType
GeneralTransform<TScalarType, NDimensions>::
GetTransform (unsigned int i)
{
   if (i >= m_TransformList.size() )
      return 0;
   return m_TransformList[i];
}

template<class TScalarType, unsigned int NDimensions>
void
GeneralTransform<TScalarType, NDimensions>::
RemoveAllTransforms()
{
   m_TransformList.clear();
   m_RemovedTransformList.clear();
}


template<class TScalarType, unsigned int NDimensions>
typename GeneralTransform<TScalarType, NDimensions>::MatrixOffsetTransformPointerType
GeneralTransform<TScalarType, NDimensions>::
GetGlobalLinearTransform() const
{

  if (!this->IsLinear())
  {
    itkExceptionMacro("Transform input is not in a linear form");
  }

  typename MatrixOffsetTransformType::Pointer ret = MatrixOffsetTransformType::New();
  ret->SetIdentity();

  typename TransformListType::const_iterator it = this->m_TransformList.begin();
  while( it != this->m_TransformList.end())
  {
      const MatrixOffsetTransformType* transform = dynamic_cast<const MatrixOffsetTransformType*>(it->GetPointer());
      if(transform)
	  ret->Compose (transform, 1);
      else 
      {
	const Self *genTrs = dynamic_cast<const Self*>(it->GetPointer());
	if(genTrs)
	  ret->Compose(genTrs->GetGlobalLinearTransform());
      }
      it++;
  }


  return ret;
}

template<class TScalarType, unsigned int NDimensions>
bool
GeneralTransform<TScalarType, NDimensions>::
IsLinear() const
{

  typename TransformListType::const_iterator it = m_TransformList.begin();

  while( it != m_TransformList.end())
  {
      const itk::Transform<double, NDimensions, NDimensions> *transform =  it->GetPointer();

      if(!transform->IsLinear())
	  return false;
      ++it;
  }

  return true;

}


} // namespace


