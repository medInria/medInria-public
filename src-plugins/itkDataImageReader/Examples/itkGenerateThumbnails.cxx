/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkImageFileReader.h>
#include "itkImageFileWriter.h"
#include <itkResampleImageFilter.h>
#include <itkImage.h>
#include "itkRGBPixel.h"
#include "itkScalarToRGBColormapImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
//#include <itkWindowedSincInterpolateImageFunction.h>
#include "itkMinimumMaximumImageCalculator.h"
#include "itkScalarImageToHistogramGenerator.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkShrinkImageFilter.h"

int main (int narg, char* arg[])
{

  if( narg<2 )
  {
    std::cerr << "Usage: " << arg[0] << " input\n";
    return -1;
  }


  const char *input = arg[1];

  typedef double ScalarType;

  typedef itk::Image<short, 3> ImageType;
  typedef itk::Image<short, 2> Image2DType;

  ImageType::Pointer image = 0;

  {
    itk::ImageFileReader<ImageType>::Pointer reader = itk::ImageFileReader<ImageType>::New();
    reader->SetFileName (input);
    try
    {
      reader->Update();
    }
    catch (itk::ExceptionObject &e)
    {
      std::cerr << e;
      return -1;
    }
    image = reader->GetOutput();
  }

  // smooth & resample
  /*
  {
    ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize();
    ImageType::SizeType newSize = size;
    newSize[0] = 72;
    newSize[1] = 72;
    ImageType::SpacingType spacing = image->GetSpacing();
    spacing[0] *= (double)(size[0])/(double)(newSize[0]);
    spacing[1] *= (double)(size[1])/(double)(newSize[1]);


    typedef itk::RecursiveGaussianImageFilter<ImageType, ImageType> FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput (image);
    filter->SetSigma(spacing[0]);
    try
    {
      filter->Update();
    }
    catch (itk::ExceptionObject &e)
    {
      std::cerr << e;
      return -1;
    }
    image = filter->GetOutput();


    typedef itk::LinearInterpolateImageFunction<ImageType, double>  InterpolatorType;
    InterpolatorType::Pointer interpolator = InterpolatorType::New();

    itk::ResampleImageFilter<ImageType, ImageType>::Pointer resampler = itk::ResampleImageFilter<ImageType, ImageType>::New();
    resampler->SetInput ( image );
    resampler->SetInterpolator( interpolator );
    resampler->SetSize( newSize );
    resampler->SetOutputSpacing( spacing );
    resampler->SetOutputOrigin( image->GetOrigin() );
    resampler->SetOutputDirection ( image->GetDirection() );

    try
    {
      resampler->Update();
    }
    catch (itk::ExceptionObject &e)
    {
      std::cerr << e;
      return -1;
    }
    image = resampler->GetOutput();

    typedef itk::ImageFileWriter<ImageType> WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( "resample.nii.gz" );
    writer->SetInput( image );
    writer->Update();
  }
  */

  // shrink
  {
    ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize();
    ImageType::SizeType newSize = size;
    newSize[0] = 128;
    newSize[1] = 128;

    unsigned int sfactor[3];
    sfactor[0] = size[0]/newSize[0];
    sfactor[1] = size[1]/newSize[1];
    sfactor[2] = size[2]/newSize[2];

    typedef itk::ShrinkImageFilter< ImageType, ImageType > FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput ( image );
    filter->SetShrinkFactors ( sfactor );
    try
    {
      filter->Update();
    }
    catch (itk::ExceptionObject &e)
    {
      std::cerr << e;
      return -1;
    }
    image = filter->GetOutput();
  }

  // min / max
  double imMin = 0.0;
  double imMax = 0.0;
  {

    typedef itk::ExtractImageFilter<ImageType, Image2DType> ExtractFilterType;
    ExtractFilterType::Pointer extractor = ExtractFilterType::New();
    extractor->SetInput ( image );
    ImageType::RegionType region = image->GetLargestPossibleRegion();
    ImageType::IndexType index = region.GetIndex();
    ImageType::SizeType ssize = region.GetSize();

    index[2] /= 2;
    ssize[2]  = 0;

    region.SetIndex ( index );
    region.SetSize ( ssize );

    extractor->SetExtractionRegion (region);
    try
    {
      extractor->Update();
    }
    catch (itk::ExceptionObject &e)
    {
      std::cerr << e;
      return -1;
    }


    typedef itk::MinimumMaximumImageCalculator<Image2DType> MinMaxCalculatorType;
    MinMaxCalculatorType::Pointer calculator = MinMaxCalculatorType::New();
    calculator->SetImage ( extractor->GetOutput() );
    try
    {
      calculator->Compute();
    }
    catch (itk::ExceptionObject &e)
    {
      std::cerr << e;
      return -1;
    }
    imMin = calculator->GetMinimum();
    imMax = calculator->GetMaximum();


    typedef itk::Statistics::ScalarImageToHistogramGenerator< Image2DType > HistogramGeneratorType;
    HistogramGeneratorType::Pointer histogramGenerator = HistogramGeneratorType::New();
    histogramGenerator->SetInput( extractor->GetOutput() );
    histogramGenerator->SetNumberOfBins( imMax - imMin + 1 );
    histogramGenerator->SetMarginalScale( 1.0 );
    histogramGenerator->SetHistogramMin( imMin );
    histogramGenerator->SetHistogramMax( imMax );
    try
    {
      histogramGenerator->Compute();
    }
    catch (itk::ExceptionObject &e)
    {
      std::cerr << e;
      return -1;
    }
    typedef HistogramGeneratorType::HistogramType  HistogramType;


    HistogramType::Pointer histogram = const_cast<HistogramType*>( histogramGenerator->GetOutput() );

    double totalFreq = histogram->GetTotalFrequency();
    std::cout << "Total freq: " << totalFreq << std::endl;

    int ind_min = 0;
    int ind_max = histogram->Size()-1;
    double freq_min = histogram->GetFrequency (ind_min)/totalFreq;
    double freq_max = histogram->GetFrequency (ind_max)/totalFreq;

    std::cout << freq_min/totalFreq << " " << freq_max/totalFreq << std::endl;

    while ( freq_min<0.01 ) {
      ind_min++;
      imMin++;
      freq_min += histogram->GetFrequency (ind_min) / totalFreq;
    }
    while ( freq_max<0.01 ) {
      ind_max--;
      imMax--;
      freq_max += histogram->GetFrequency (ind_max) / totalFreq;
    }

    std::cout << imMin << " " << imMax << std::endl;
  }


  // color
  typedef itk::RGBPixel<unsigned char>    RGBPixelType;
  typedef itk::Image<RGBPixelType, 3>     RGBImageType;
  typedef itk::ScalarToRGBColormapImageFilter<ImageType, RGBImageType> RGBFilterType;
  RGBFilterType::Pointer rgbfilter = RGBFilterType::New();
  rgbfilter->SetColormap( RGBFilterType::Grey );
  rgbfilter->GetColormap()->SetMinimumRGBComponentValue( 0 );
  rgbfilter->GetColormap()->SetMaximumRGBComponentValue( 255 );
  rgbfilter->UseInputImageExtremaForScalingOff ();
  rgbfilter->GetColormap()->SetMinimumInputValue ( imMin );
  rgbfilter->GetColormap()->SetMaximumInputValue ( imMax );
  rgbfilter->SetInput( image );
  try
  {
    rgbfilter->Update();
  }
  catch (itk::ExceptionObject &e)
  {
    std::cerr << e;
    return -1;
  }


  typedef itk::Image<RGBPixelType, 2>                           RGBImage2DType;
  typedef itk::ExtractImageFilter<RGBImageType, RGBImage2DType> RGBExtractFilterType;
  RGBExtractFilterType::Pointer extractor = RGBExtractFilterType::New();
  extractor->SetInput ( rgbfilter->GetOutput() );
  ImageType::SizeType size = rgbfilter->GetOutput()->GetLargestPossibleRegion().GetSize();
  typedef itk::ImageFileWriter<RGBImage2DType> WriterType;
  WriterType::Pointer writer = WriterType::New();

  for( unsigned int i=0; i<size[2]; i++ )
  {
    RGBImageType::RegionType region = rgbfilter->GetOutput()->GetLargestPossibleRegion();
    RGBImageType::IndexType index = region.GetIndex();
    RGBImageType::SizeType ssize = region.GetSize();

    index[2] = i;
    ssize[2] = 0;

    region.SetIndex ( index );
    region.SetSize ( ssize );

    extractor->SetExtractionRegion (region);
    try
    {
      extractor->Update();
    }
    catch (itk::ExceptionObject &e)
    {
      std::cerr << e;
      return -1;
    }


    char filename[256];
    sprintf ( filename, "tmb/tumbnail%.2d.jpg", i);
    writer->SetFileName( filename );
    writer->SetInput( extractor->GetOutput() );
    writer->Update();

  }

  return 0;

}
