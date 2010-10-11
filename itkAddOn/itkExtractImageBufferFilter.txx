/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkExtractImageBufferFilter.txx,v $
  Language:  C++
  Date:      $Date: 2008-11-09 16:51:04 $
  Version:   $Revision: 1.28 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkExtractImageBufferFilter_txx
#define __itkExtractImageBufferFilter_txx

#include "itkExtractImageBufferFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkObjectFactory.h"
#include "itkExtractImageBufferFilterRegionCopier.h"
#include "itkProgressReporter.h"


namespace itk
{

/**
 *
 */
template <class TInputImage, class TOutputImage>
ExtractImageBufferFilter<TInputImage,TOutputImage>
::ExtractImageBufferFilter() 
{
}

/** 
 * ExtractImageBufferFilter can be implemented as a multithreaded filter.
 * Therefore, this implementation provides a ThreadedGenerateData()
 * routine which is called for each processing thread. The output
 * image data is allocated automatically by the superclass prior to
 * calling ThreadedGenerateData().  ThreadedGenerateData can only
 * write to the portion of the output image specified by the
 * parameter "outputRegionForThread"
 *
 * \sa ImageToImageFilter::ThreadedGenerateData(),
 *     ImageToImageFilter::GenerateData() 
 */
template <class TInputImage, class TOutputImage>
void 
ExtractImageBufferFilter<TInputImage,TOutputImage>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                       int threadId)
{
  itkDebugMacro(<<"Actually executing");

  // Get the input and output pointers
  typename Superclass::InputImageConstPointer  inputPtr = this->GetInput();
  typename Superclass::OutputImagePointer outputPtr = this->GetOutput();

  // support progress methods/callbacks
  ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels());
  
  // Define the portion of the input to walk for this thread
  InputImageRegionType inputRegionForThread;
  this->CallCopyOutputRegionToInputRegion(inputRegionForThread, outputRegionForThread);

  const typename TInputImage::OffsetValueType *offset = inputPtr->GetOffsetTable();
  
  unsigned int dimension = TInputImage::ImageDimension-1;  
  typename TInputImage::PixelType *buffer = const_cast<typename TInputImage::PixelType*>(inputPtr->GetBufferPointer());

  for (unsigned int i=0; i<TInputImage::ImageDimension; i++)
	buffer += offset[i] * this->GetExtractionRegion().GetIndex()[i];

  ///\warning this will fail if less than offset[dimension] pixels are remaining after extration region index
  outputPtr->GetPixelContainer()->SetImportPointer( buffer, offset[dimension], false );

}

} // end namespace itk

#endif
