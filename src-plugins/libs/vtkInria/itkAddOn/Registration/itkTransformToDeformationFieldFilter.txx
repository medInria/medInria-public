/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "itkTransformToDeformationFieldFilter.h"
#include "itkProgressReporter.h"
#include "itkImageRegionIteratorWithIndex.h"

namespace itk
{

/**
 * Initialize new instance
 */
template <class TOutputImage, class TTransformScalarType>
TransformToDeformationFieldFilter<TOutputImage,TTransformScalarType>
::TransformToDeformationFieldFilter()
{
  m_OutputSpacing.Fill(1.0);
  m_OutputOrigin.Fill(0.0);

  m_OutputDirection.SetIdentity();

  m_Transform = 0;
}


/**
 * Print out a description of self
 *
 * \todo Add details about this class
 */
template <class TOutputImage, class TTransformScalarType>
void 
TransformToDeformationFieldFilter<TOutputImage,TTransformScalarType>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "OutputRegion:    " << m_OutputRegion << std::endl;
  os << indent << "OutputSpacing:   " << m_OutputSpacing << std::endl;
  os << indent << "OutputOrigin:    " << m_OutputOrigin << std::endl;
  os << indent << "OutputDirection:    " << m_OutputDirection << std::endl;
  os << indent << "Transform: " << m_Transform.GetPointer() << std::endl;

  return;
}



/**
 * Set the output image spacing.
 */
template <class TOutputImage, class TTransformScalarType>
void 
TransformToDeformationFieldFilter<TOutputImage,TTransformScalarType>
::SetOutputSpacing(
  const double* spacing)
{
  SpacingType s(spacing);
  this->SetOutputSpacing( s );
}


/**
 * Set the output image origin.
 */
template <class TOutputImage, class TTransformScalarType>
void 
TransformToDeformationFieldFilter<TOutputImage,TTransformScalarType>
::SetOutputOrigin(
  const double* origin)
{
  OriginPointType p(origin);
  this->SetOutputOrigin( p );
}


/**
 * GenerateData
 */
template <class TOutputImage, class TTransformScalarType>
void 
TransformToDeformationFieldFilter<TOutputImage,TTransformScalarType>
::GenerateData()
{
  itkDebugMacro(<<"Actually executing");
  
  // Sanity checks
  if ( !m_Transform )
    {
    itkExceptionMacro(<<"Transform is not present" );
    }

  // Get the output pointers
  OutputImageType *  outputPtr = this->GetOutput();

  outputPtr->SetBufferedRegion( outputPtr->GetRequestedRegion() );
  outputPtr->Allocate();

  // Create an iterator that will walk the output region for this thread.
  typedef ImageRegionIteratorWithIndex< 
                                  TOutputImage> OutputIterator;

  OutputImageRegionType region = outputPtr->GetRequestedRegion();

  OutputIterator outIt( outputPtr, region );

  // Define a few indices that will be used to translate from an input pixel
  // to an output pixel
  //OutputIndexType outputIndex;         // Index to current output pixel

  typedef typename TransformType::InputPointType  InputPointType;
  typedef typename TransformType::OutputPointType OutputPointType;

  InputPointType inputPoint;    // Coordinates of current input pixel
  OutputPointType outputPoint;  // Coordinates of current output pixel
  OutputPixelType displacement; // Displacement of current input pixel

  // Support for progress methods/callbacks
  ProgressReporter progress(this, 0, region.GetNumberOfPixels(), 10);
        
  outIt.GoToBegin();

  // Walk the output region
  while ( !outIt.IsAtEnd() )
    {
    // Determine the position of the current pixel
    outputPtr->TransformIndexToPhysicalPoint(outIt.GetIndex(), inputPoint);

    // Compute corresponding displacement vector
    outputPoint = m_Transform->TransformPoint( inputPoint );

    
    for( unsigned int i=0; i < ImageDimension; i++)
      {
      displacement[i] = outputPoint[i] - inputPoint[i];
      }

    outIt.Set( displacement );
    ++outIt;
    progress.CompletedPixel();
    }

  return;
}


/** 
 * Inform pipeline of required output region
 */
template <class TOutputImage, class TTransformScalarType>
void 
TransformToDeformationFieldFilter<TOutputImage,TTransformScalarType>
::GenerateOutputInformation()
{
  // call the superclass' implementation of this method
  Superclass::GenerateOutputInformation();

  // get pointers to the input and output
  OutputImagePointer outputPtr = this->GetOutput();
  if ( !outputPtr )
    {
    return;
    }

  // Set the size of the output region
  outputPtr->SetLargestPossibleRegion( m_OutputRegion );

  // Set spacing and origin
  outputPtr->SetSpacing( m_OutputSpacing );
  outputPtr->SetOrigin( m_OutputOrigin );
  outputPtr->SetDirection( m_OutputDirection );

  return;
}



/** 
 * Verify if any of the components has been modified.
 */
template <class TOutputImage, class TTransformScalarType>
unsigned long 
TransformToDeformationFieldFilter<TOutputImage,TTransformScalarType>
::GetMTime() const
{
  unsigned long latestTime = Object::GetMTime(); 

  if( m_Transform )
    {
    if( latestTime < m_Transform->GetMTime() )
      {
      latestTime = m_Transform->GetMTime();
      }
    }

  return latestTime;
}

} // end namespace itk


