/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "itkImageSeriesReshapeFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

namespace itk
{

	
  // ----------------------------------------------------------------------
  // Constructor
  template <class TInputImage, class TOutputImage>
  ImageSeriesReshapeFilter<TInputImage, TOutputImage>
  ::ImageSeriesReshapeFilter()
  {
    m_InterlaceSpacing = itk::NumericTraits<float>::min();
  }


  // ----------------------------------------------------------------------
  // Destructor
  template <class TInputImage, class TOutputImage>
  ImageSeriesReshapeFilter<TInputImage, TOutputImage>
  ::~ImageSeriesReshapeFilter()
  {
    this->RemoveAllOutputs();
  }

  
  // ----------------------------------------------------------------------
  // PrintSelf
  template <class TInputImage, class TOutputImage>
  void ImageSeriesReshapeFilter<TInputImage, TOutputImage>
  ::PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }


  // ----------------------------------------------------------------------
  // RemoveAllOutputs
  template <class TInputImage, class TOutputImage>
  void ImageSeriesReshapeFilter<TInputImage, TOutputImage>
  ::RemoveAllOutputs()
  {
    for (unsigned int i = 0; i < m_OutputImageList.size(); i++)
      m_OutputImageList[i]->UnRegister();
		
    m_OutputImageList.clear();
  }

  
  // ----------------------------------------------------------------------
  // GenerateData
  template <class TInputImage, class TOutputImage>
  void ImageSeriesReshapeFilter<TInputImage, TOutputImage>
  ::GenerateData()
  {
    this->UpdateProgress(0.0); 

    this->RemoveAllOutputs();

    if( ImageDimension != 3 )
      throw itk::ExceptionObject(__FILE__,__LINE__,"Image Dimension not Supported.");
		
    unsigned int zDim = m_InputImageList.size();

    if (zDim < 2)
      throw itk::ExceptionObject(__FILE__,__LINE__,"No input images.");
		
    typename InputImageType::Pointer              image0     = m_InputImageList[0];
    typename InputImageType::RegionType::SizeType size0      = image0->GetLargestPossibleRegion().GetSize();
    typename InputImageType::SpacingType          spacing0   = image0->GetSpacing();
    typename InputImageType::DirectionType        direction0 = image0->GetDirection();
    typename InputImageType::PointType            origin0    = image0->GetOrigin();

    for (unsigned int i = 1; i < zDim; i++)
    {
      typename InputImageType::Pointer              image   = m_InputImageList[i];
      typename InputImageType::RegionType::SizeType size    = image->GetLargestPossibleRegion().GetSize();
      typename InputImageType::SpacingType          spacing = image->GetSpacing();

      // Images must have same size
      if (size != size0)
	throw itk::ExceptionObject(__FILE__,__LINE__,"Input images must have the same size.");

      // Images must have same spacing
      if (spacing != spacing0)
	throw itk::ExceptionObject(__FILE__,__LINE__,"Input images must have the same size.");
			
    }
			
    unsigned int xDim = size0[0];
    unsigned int yDim = size0[1];
    unsigned int tDim = size0[2];
		
    // Allocate T output images of dim XxYxZ

    float origin[TOutputImage::ImageDimension];
    origin[0] = origin0[0];
    origin[1] = origin0[1];
    origin[2] = origin0[2];

    float spacing[TOutputImage::ImageDimension];
    spacing[0] = spacing0[0];
    spacing[1] = spacing0[1];
    spacing[2] = m_InterlaceSpacing;

    itk::Vector<double, TOutputImage::ImageDimension> vector;
    vector[0] = 0;
    vector[1] = 0;
    vector[2] = spacing0[2];

    vector = direction0 * vector;

//     for (unsigned int j=0; j<TOutputImage::ImageDimension; j++)
// 	origin[j] = origin[j] - vector[j];
    typename OutputImageType::RegionType::SizeType size;
    size[0] = xDim;
    size[1] = yDim;
    size[2] = zDim;
		
    typedef typename TOutputImage::IndexType IndexType;
    IndexType start;
    start.Fill(0);
				
    typename TOutputImage::RegionType region;
    region.SetSize(size);
    region.SetIndex(start);

    for (unsigned int i = 0; i < tDim; i++)
    {
      typename TOutputImage::Pointer  OutputImage = TOutputImage::New();
      
      OutputImage->SetSpacing(spacing);       // Set image spacing
      OutputImage->SetOrigin(origin);         // Set image origin
      // I don't think we need this direction information to be passed
      // from input to output
      
      OutputImage->SetDirection(direction0);  // Set image direction
      OutputImage->SetRegions(region);
      OutputImage->Allocate();

      m_OutputImageList.push_back(OutputImage);
      OutputImage->Register(); // Increment the reference count

      for (unsigned int j=0; j<TOutputImage::ImageDimension; j++)
	origin[j] = origin[j] + vector[j];
    }

    typename TInputImage::RegionType           regiontocrop;
    typename TInputImage::RegionType::SizeType sizetocrop;
    typename TInputImage::IndexType            indextocrop;

    indextocrop[0] = 0;
    indextocrop[1] = 0;

    sizetocrop[0] = xDim;
    sizetocrop[1] = yDim;
    sizetocrop[2] = 1;

    regiontocrop.SetSize(sizetocrop);

    typename TOutputImage::RegionType           regiontofill;
    typename TOutputImage::RegionType::SizeType sizetofill;
    typename TOutputImage::IndexType            indextofill;
		
    indextofill[0] = 0;
    indextofill[1] = 0;

    sizetofill[0] = xDim;
    sizetofill[1] = yDim;
    sizetofill[2] = 1;
		
    regiontofill.SetSize(sizetofill);


    // Build the volumes
    for (unsigned int t = 0; t < tDim; t++)
    {
      for (unsigned int z = 0; z<zDim; z++)
      {
	indextocrop[2] = t;
	regiontocrop.SetIndex(indextocrop);

	indextofill[2] = z;
	regiontofill.SetIndex(indextofill);
	
	/*
	  croper m_InputImageList[z] suivant 
	  origin 0 0 t
	  size xDim yDim 1
							
	  a inserer a OutputImageList[t] a
	  origin 0 0 z
	  size xDim yDim 1
	*/
	
	ImageRegionIterator<TInputImage> it(m_InputImageList[z], regiontocrop);
	ImageRegionIterator<TOutputImage> ot(m_OutputImageList[t], regiontofill);

	it.GoToBegin();
	ot.GoToBegin();
	while( !it.IsAtEnd() )
	{
	  ot.Set(it.Get());
	  ++it;
	  ++ot;
	}						
      }
    }

    this->UpdateProgress(1.0); 
  }


  
} // end namespace itk


