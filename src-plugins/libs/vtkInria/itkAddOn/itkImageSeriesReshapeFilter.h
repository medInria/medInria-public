/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "itkObjectFactory.h"
#include "itkImageToImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkConstNeighborhoodIterator.h"
#include <string>
#include "itkSize.h"


#include <vector>

namespace itk
{

  template <class TInputImage, class TOutputImage>
    class ITK_EXPORT ImageSeriesReshapeFilter:
  public ImageToImageFilter < TInputImage, TOutputImage>
  {
    
  public:
    typedef ImageSeriesReshapeFilter          Self;
    typedef ImageToImageFilter<TInputImage, TOutputImage>    Superclass;
    typedef SmartPointer<Self>       Pointer;
    typedef SmartPointer<const Self> ConstPointer;


    itkNewMacro(Self);
    itkTypeMacro (ImageSeriesReshapeFilter, ImageToImageFilter);

    itkStaticConstMacro(ImageDimension, unsigned int,
			TOutputImage::ImageDimension);

    itkGetMacro(InterlaceSpacing, float);
    itkSetMacro(InterlaceSpacing, float);

    /** Image typedefs support */
    typedef TInputImage                                InputImageType;
    typedef TOutputImage                               OutputImageType;

    typedef std::vector<typename InputImageType::Pointer>       InputImageListType;
    typedef std::vector<typename OutputImageType::Pointer>      OutputImageListType;
    
    /** Pixel typedefs */
    typedef typename TInputImage::PixelType            InputPixelType;
    typedef typename TOutputImage::PixelType           OutputPixelType;

    /** Iterator typedefs */
    typedef ImageRegionConstIterator<InputImageType>   InputIteratorType;
    typedef ImageRegionIterator<OutputImageType>       OutputIteratorType;


    /** Index typedefs */
    typedef typename InputIteratorType::IndexType      InputIndexType;
    typedef typename OutputIteratorType::IndexType     OutputIndexType;


    /** Neighborhood typedefs */

    typedef typename TOutputImage::RegionType                  OutputImageRegionType;

    void SetInputImageList(const InputImageListType list)
    {
      m_InputImageList = list;
    }

    OutputImageListType GetOutputImageList()
    {
      return m_OutputImageList;
    }

    void RemoveAllOutputs();

  protected:
    ImageSeriesReshapeFilter();
    ~ImageSeriesReshapeFilter();
    void PrintSelf(std::ostream& os, Indent indent) const;
    
    void GenerateData(); 
    
  private:
    ImageSeriesReshapeFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

    InputImageListType m_InputImageList;
    OutputImageListType m_OutputImageList;
		
    float m_InterlaceSpacing;
  };
  
  
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkImageSeriesReshapeFilter.txx"
#endif
  

