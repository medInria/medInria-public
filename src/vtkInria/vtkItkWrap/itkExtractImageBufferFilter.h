/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkExtractImageBufferFilter.h,v $
  Language:  C++
  Date:      $Date: 2009-04-25 12:27:21 $
  Version:   $Revision: 1.16 $
  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.
  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notices for more information.
=========================================================================*/

#pragma once

#include "itkExtractImageFilter.h"
#include "itkSmartPointer.h"

namespace itk
{

/** \class ExtractImageBufferFilter
 * \sa CropImageFilter
 * \ingroup GeometricTransforms
 */
template <class TInputImage, class TOutputImage>
class ExtractImageBufferFilter:
    public ExtractImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef ExtractImageBufferFilter                            Self;
  typedef ExtractImageFilter<TInputImage,TOutputImage>  Superclass;
  typedef SmartPointer<Self>                            Pointer;
  typedef SmartPointer<const Self>                      ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  /** Run-time type information (and related methods). */
  itkTypeMacro(ExtractImageBufferFilter, ExtractImageFilter);

  /** Typedef to describe the output and input image region types. */
  typedef typename TOutputImage::RegionType OutputImageRegionType;
  typedef typename TInputImage::RegionType  InputImageRegionType;

protected:
  ExtractImageBufferFilter();
  ~ExtractImageBufferFilter() {};

  void GenerateData();

private:
  ExtractImageBufferFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
};

  
} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkExtractImageBufferFilter.txx"
#endif
  

