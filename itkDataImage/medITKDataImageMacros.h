#ifndef _med_ITKDataImageMacros_h_
#define _med_ITKDataImageMacros_h_


#define medImplementITKDataImage(type, dimension, suffix)		\
  class itkDataImage##suffix##Private					\
  {									\
  public:								\
    typedef type                                                          ScalarType; \
    typedef itk::Image<ScalarType, dimension>                             ImageType; \
    typedef itk::Statistics::ScalarImageToHistogramGenerator< ImageType > HistogramGeneratorType; \
    typedef HistogramGeneratorType::HistogramType                         HistogramType; \
  public:								\
    ImageType::Pointer          image;					\
    ScalarType range_min;						\
    ScalarType range_max;						\
    HistogramType::Pointer histogram;					\
    int                    histogram_min;				\
    int                    histogram_max;				\
  };									\
  itkDataImage##suffix::itkDataImage##suffix(): dtkAbstractDataImage(), d (new itkDataImage##suffix##Private) \
  {									\
    d->image = 0;							\
    d->histogram = 0;							\
    d->range_min = 0;							\
    d->range_max = 0;							\
    d->histogram_min = 0;						\
    d->histogram_max = 0;						\
  }									\
  itkDataImage##suffix::~itkDataImage##suffix()				\
  {									\
  }									\
  bool itkDataImage##suffix::registered()				\
  {									\
    return dtkAbstractDataFactory::instance()->registerDataType("itkDataImage"#suffix, createItkDataImage##suffix); \
  }									\
  QString itkDataImage##suffix::description() const			\
  {									\
    return "itkDataImage"#suffix;					\
  }									\
  void itkDataImage##suffix::setData(void *data)			\
  {									\
    typedef itkDataImage##suffix##Private::ImageType ImageType;		\
    ImageType::Pointer image = dynamic_cast<ImageType*>( (itk::Object*) data ); \
    d->image = image;							\
  }									\
  void *itkDataImage##suffix::output(void)				\
  {									\
    return d->image.GetPointer();					\
  }									\
  void *itkDataImage##suffix::data(void)				\
  {									\
    return d->image.GetPointer();					\
  }									\
  void itkDataImage##suffix::update(void)				\
  {									\
  }									\
  int itkDataImage##suffix::xDimension(void)				\
  {									\
    return d->image->GetLargestPossibleRegion().GetSize()[0];		\
  }									\
  int itkDataImage##suffix::yDimension(void)				\
  {									\
    return d->image->GetLargestPossibleRegion().GetSize()[1];		\
  }									\
  int itkDataImage##suffix::zDimension(void)				\
  {									\
    return d->image->GetLargestPossibleRegion().GetSize()[2];		\
  }									\
  int itkDataImage##suffix::minRangeValue(void)				\
  {									\
    return d->range_min;						\
  }									\
  int itkDataImage##suffix::maxRangeValue(void)				\
  {									\
    return d->range_max;						\
  }									\
  int itkDataImage##suffix::scalarValueCount(int value)			\
  {									\
    typedef itkDataImage##suffix##Private::ScalarType ScalarType;	\
    if( !d->histogram.IsNull() )					\
    {									\
      if( (ScalarType)value>=d->range_min && (ScalarType)value<=d->range_max ) \
    {									\
      return d->histogram->GetFrequency( value, 0 );			\
    }									\
  }									\
  return -1;								\
  }									\
  int itkDataImage##suffix::scalarValueMinCount(void)                   \
  {									\
    return d->histogram_min;						\
  }									\
  int itkDataImage##suffix::scalarValueMaxCount(void)			\
  {									\
    return d->histogram_max;						\
  }									\
  dtkAbstractData *createItkDataImage##suffix(void)			\
  {									\
    return new itkDataImage##suffix;					\
  }


#endif



// if(!image.IsNull()) {							
//  d->image = image;							
//  typedef itk::MinimumMaximumImageCalculator<ImageType> MinMaxCalculatorType; 
//  MinMaxCalculatorType::Pointer calculator = MinMaxCalculatorType::New(); 
//  calculator->SetImage ( image );					
//  try									
//  {									
//    calculator->Compute();						
//  }									
//  catch (itk::ExceptionObject &e)					
//  {									
//    std::cerr << e;							
//    return;								
//  }									
//  d->range_min = calculator->GetMinimum();				
//  d->range_max = calculator->GetMaximum();				
//  std::cout << "Image min/max: " << d->range_min << " " << d->range_max << std::endl; 
// }									

//typedef itkDataImage##suffix##Private::HistogramGeneratorType HistogramGeneratorType; 
//HistogramGeneratorType::Pointer histogramGenerator = HistogramGeneratorType::New(); 
//histogramGenerator->SetInput( image );				
//histogramGenerator->SetNumberOfBins( d->range_max - d->range_min + 1 ); 
//histogramGenerator->SetMarginalScale( 1.0 );				
//histogramGenerator->SetHistogramMin( d->range_min );			
//histogramGenerator->SetHistogramMax( d->range_max );			
//try									
//{									
//histogramGenerator->Compute();					
//}									
//catch (itk::ExceptionObject &e)					
//{									
//std::cerr << e;							
//return;								
//}									
//typedef HistogramGeneratorType::HistogramType  HistogramType;		
//d->histogram = const_cast<HistogramType*>( histogramGenerator->GetOutput() ); 
//d->histogram_min = d->histogram->GetFrequency( d->range_min, 0 );	
//d->histogram_max = d->histogram->GetFrequency( d->range_max, 0 );	
