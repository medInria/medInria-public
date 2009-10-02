#include "itkDataImage.h"

#include "medItk.h"

#include <itkImage.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkScalarImageToHistogramGenerator.h>

#include <dtkCore/dtkAbstractDataFactory.h>

#include "medITKDataImageMacros.h"


medImplementITKDataImage (char,           3, Char3);
medImplementITKDataImage (unsigned char,  3, UChar3);
medImplementITKDataImage (short,          3, Short3);
medImplementITKDataImage (unsigned short, 3, UShort3);
medImplementITKDataImage (int,            3, Int3);
medImplementITKDataImage (unsigned int,   3, UInt3);
medImplementITKDataImage (long,           3, Long3);
medImplementITKDataImage (unsigned long,  3, ULong3);
medImplementITKDataImage (float,          3, Float3);
medImplementITKDataImage (double,         3, Double3);

















/*
class itkDataImageShort3Private
{
  
public:
  typedef short                                                         ScalarType;
  typedef itk::Image<ScalarType, 3> 		                        ImageType;
  typedef itk::Statistics::ScalarImageToHistogramGenerator< ImageType > HistogramGeneratorType;
  typedef HistogramGeneratorType::HistogramType                         HistogramType;
  
public:
  ImageType::Pointer          image;
  
  ScalarType range_min;
  ScalarType range_max;

  HistogramType::Pointer histogram;
  int                    histogram_min;
  int                    histogram_max;
};


itkDataImageShort3::itkDataImageShort3(): dtkAbstractDataImage(), d (new itkDataImageShort3Private)
{
  d->image = 0;
}


itkDataImageShort3::~itkDataImageShort3()
{
}


bool itkDataImageShort3::registered()
{
  return dtkAbstractDataFactory::instance()->registerDataType("itkDataImageShort3", createItkDataImageShort3);
}


QString itkDataImageShort3::description() const
{
  return "itkDataImageShort3";
}


void itkDataImageShort3::setData(void *data)
{
  typedef itkDataImageShort3Private::ImageType ImageType;
  ImageType::Pointer image = dynamic_cast<ImageType*>( (itk::Object*) data );
  
  if(!image.IsNull()) {
    
    d->image = image;
    
    typedef itk::MinimumMaximumImageCalculator<ImageType> MinMaxCalculatorType;
    
    MinMaxCalculatorType::Pointer calculator = MinMaxCalculatorType::New();
    calculator->SetImage ( image );
    try
    {
      calculator->Compute();
    }
    catch (itk::ExceptionObject &e)
    {
      std::cerr << e;
      return;
    }
    
    d->range_min = calculator->GetMinimum();
    d->range_max = calculator->GetMaximum();


    typedef itkDataImageShort3Private::HistogramGeneratorType HistogramGeneratorType;
    HistogramGeneratorType::Pointer histogramGenerator = HistogramGeneratorType::New();
    histogramGenerator->SetInput( image );
    histogramGenerator->SetNumberOfBins( d->range_max - d->range_min + 1 );
    histogramGenerator->SetMarginalScale( 1.0 );    
    histogramGenerator->SetHistogramMin( d->range_min );
    histogramGenerator->SetHistogramMax( d->range_max );
    try
    {
      histogramGenerator->Compute();
    }
    catch (itk::ExceptionObject &e)
    {
      std::cerr << e;
      return;
    }
    typedef HistogramGeneratorType::HistogramType  HistogramType;
    d->histogram = const_cast<HistogramType*>( histogramGenerator->GetOutput() );

    d->histogram_min = d->histogram->GetFrequency( d->range_min, 0 );
    d->histogram_max = d->histogram->GetFrequency( d->range_max, 0 );

    std::cout << "Image min/max: " << d->range_min << " " << d->range_max << std::endl;
  }
}


void *itkDataImageShort3::output(void)
{
  return d->image.GetPointer();
}


void *itkDataImageShort3::data(void)
{
  return d->image.GetPointer();
}


void itkDataImageShort3::update(void)
{
}


int itkDataImageShort3::xDimension(void)
{
  return d->image->GetLargestPossibleRegion().GetSize()[0];
}


int itkDataImageShort3::yDimension(void)
{
  return d->image->GetLargestPossibleRegion().GetSize()[1];
}


int itkDataImageShort3::zDimension(void)
{
  return d->image->GetLargestPossibleRegion().GetSize()[2];
}


int itkDataImageShort3::minRangeValue(void)
{
  return d->range_min;
}

int itkDataImageShort3::maxRangeValue(void)
{
  return d->range_max;
}

int itkDataImageShort3::scalarValueCount(int value)
{
  typedef itkDataImageShort3Private::ScalarType ScalarType;
  if( !d->histogram.IsNull() ){
    if( (ScalarType)value>=d->range_min && (ScalarType)value<=d->range_max )
      return d->histogram->GetFrequency( value, 0 );
  }
  return -1;
}

int itkDataImageShort3::scalarValueMinCount(void)
{
  return d->histogram_min;
}

int itkDataImageShort3::scalarValueMaxCount(void)
{
  return d->histogram_max;
}

dtkAbstractData *createItkDataImageShort3(void)
{
  return new itkDataImageShort3;
}
*/
