#ifndef _med_ITKDataImageMacros_h_
#define _med_ITKDataImageMacros_h_

#include <itkVector.h>
#include <itkImage.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkScalarImageToHistogramGenerator.h>
#include <itkRGBPixel.h>
#include <itkGreyColormapFunctor.h>
#include <itkScalarToRGBColormapImageFilter.h>
#include <itkRegionOfInterestImageFilter.h>
#include <itkExtractImageFilter.h>
#include <itkShrinkImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkRecursiveGaussianImageFilter.h>

#include <dtkCore/dtkAbstractDataFactory.h>

#include <QtGui>

template< typename TPixel, int VDimension >
void
generateThumbnails (typename itk::Image<TPixel, VDimension>::Pointer image,
		    int xydim, bool singlez, QList<QImage> & thumbnails)
{
  if (VDimension < 3)
    return;
  if (image.IsNull())
    return;

  typedef itk::Image<TPixel, VDimension> ImageType;
  typename ImageType::Pointer  img  = image;
  typename ImageType::SizeType size = img->GetLargestPossibleRegion().GetSize();

  if ((( singlez && thumbnails.length() == 1) ||
       (!singlez && thumbnails.length() == static_cast< int >( size[2] ))) &&
      thumbnails.length()    >  1     &&
      thumbnails[0].height() == xydim &&
      thumbnails[0].width()  == xydim)
    return;

  thumbnails.clear();

  qDebug() << "generating thumbnails...";

  if (singlez)
  {
    typename ImageType::SizeType newSize = size;
    newSize[2]    = 1;
    typename ImageType::IndexType index;
    index.Fill( 0 );
    index[2] = size[2] / 2;
    typename ImageType::RegionType region = img->GetLargestPossibleRegion(); 
    region.SetIndex( index );
    region.SetSize( newSize );

    typedef itk::RegionOfInterestImageFilter<ImageType, ImageType> FilterType;
    typename FilterType::Pointer extractor = FilterType::New();
    extractor->SetInput( img );
    extractor->SetRegionOfInterest( region );
    try
    {
      extractor->Update();
    }
    catch (itk::ExceptionObject &e)
    {
      qDebug() << e.GetDescription();
      return;
    }

    img = extractor->GetOutput();
    size = img->GetLargestPossibleRegion().GetSize();
  }

  if (size[0] > static_cast<unsigned int>(xydim) ||
      size[1] > static_cast<unsigned int>(xydim) )
  {
    typename ImageType::SizeType    newSize    = size;
    typename ImageType::SpacingType spacing    = img->GetSpacing();
    typename ImageType::SpacingType newSpacing = spacing;

    newSize[0] = 128;
    newSize[1] = 128;

    typename ImageType::SpacingType sfactor;
    typename ImageType::SpacingType sigma, variance;
    for (unsigned int i = 0; i < VDimension; ++i)
    {
      sfactor[i]     = (double)size[i] / (double)newSize[i];
      newSpacing[i] *= sfactor[i];
      sigma[i]       = 0.5 * sfactor[i];
    }
    int index	       = size[0] > size[1] ? 0 : 1;
    sfactor[!index]    = sfactor[index];
    variance[!index]   = variance[index];
    newSpacing[!index] = newSpacing[index];

    typename ImageType::PointType origin = img->GetOrigin();
    origin[!index] -= 0.5 * (newSize[!index] * newSpacing[!index] -
			     size[!index] * spacing[!index]);

    typedef itk::Image<float, VDimension> FloatImageType;
    typedef itk::RecursiveGaussianImageFilter<ImageType, FloatImageType>
      SmootherType0;
    typedef itk::RecursiveGaussianImageFilter<FloatImageType, FloatImageType>
      SmootherType1;
    typename SmootherType0::Pointer smoother0 = SmootherType0::New();
    typename SmootherType1::Pointer smoother1 = SmootherType1::New();
    smoother0->SetSigma ( sigma[0] * spacing[0]);
    smoother1->SetSigma ( sigma[1] * spacing[1]);
    smoother0->SetDirection (0);
    smoother1->SetDirection (1);
    smoother0->SetOrder (SmootherType0::ZeroOrder);
    smoother1->SetOrder (SmootherType1::ZeroOrder);
    smoother0->SetInput( img );
    smoother1->SetInput( smoother0->GetOutput() );
    typename SmootherType1::Pointer smoother = smoother1;

    typedef typename itk::ResampleImageFilter<FloatImageType, ImageType>
      FilterType;
    typename FilterType::Pointer resampler = FilterType::New();
    resampler->SetInput (smoother->GetOutput());
    resampler->SetSize (newSize);
    resampler->SetOutputSpacing (newSpacing);
    resampler->SetOutputOrigin (origin);
    resampler->SetOutputDirection (img->GetDirection());

    try
    {
      resampler->Update();
    }
    catch (itk::ExceptionObject &e)
    {
      qDebug() << e.GetDescription();
      return;
    }

    img = resampler->GetOutput();
  }

  typedef itk::RGBPixel<unsigned char>        RGBPixelType;
  typedef itk::Image<RGBPixelType, VDimension> RGBImageType;
  typedef itk::ScalarToRGBColormapImageFilter<ImageType, RGBImageType>
    RGBFilterType;
  typename RGBFilterType::Pointer rgbfilter = RGBFilterType::New();
  rgbfilter->SetColormap (RGBFilterType::Grey);
  rgbfilter->GetColormap()->SetMinimumRGBComponentValue (0);
  rgbfilter->GetColormap()->SetMaximumRGBComponentValue (255);
  rgbfilter->UseInputImageExtremaForScalingOn ();
  rgbfilter->SetInput (img);

  try
  {
    rgbfilter->Update();
  }
  catch (itk::ExceptionObject &e)
  {
    qDebug() << e.GetDescription();
    return;
  }

  thumbnails.clear();

  size = rgbfilter->GetOutput()->GetLargestPossibleRegion().GetSize();
  unsigned long	 nvoxels_per_slice = size[0] * size[1];
  unsigned long	 voxelCount	   = 0;
  QImage	*qimage		   = new QImage (size[0], size[1],
						 QImage::Format_ARGB32);
  uchar		*qImageBuffer	   = qimage->bits();
  itk::ImageRegionIterator<RGBImageType> it (
    rgbfilter->GetOutput(),
    rgbfilter->GetOutput()->GetLargestPossibleRegion());

  while (!it.IsAtEnd())
  {
    *qImageBuffer++ = static_cast<unsigned char>(it.Value().GetRed());
    *qImageBuffer++ = static_cast<unsigned char>(it.Value().GetGreen());
    *qImageBuffer++ = static_cast<unsigned char>(it.Value().GetBlue());
    *qImageBuffer++ = 0xFF;

    ++it;
    ++voxelCount;

    if ((voxelCount % nvoxels_per_slice) ==  0)
    {
      thumbnails.push_back (
	qimage->mirrored(img->GetDirection()(0,0) == -1.0,
			 img->GetDirection()(1,1) == -1.0));
      qimage = new QImage (size[0], size[1], QImage::Format_ARGB32);
      qImageBuffer = qimage->bits();
    }
  }
  qDebug() << "...generated " << thumbnails.length() << " thumbnails";
}

#define medImplementITKDataImage(type, dimension, suffix)		\
  class itkDataImage##suffix##Private					\
  {									\
  public:								\
    typedef type                              ScalarType;		\
    typedef itk::Image<ScalarType, dimension> ImageType;		\
    typedef itk::Statistics::ScalarImageToHistogramGenerator< ImageType > \
      HistogramGeneratorType;						\
    typedef HistogramGeneratorType::HistogramType HistogramType;	\
  public:								\
    ImageType::Pointer          image;					\
    bool range_computed;						\
    ScalarType range_min;						\
    ScalarType range_max;						\
    HistogramType::Pointer histogram;					\
    int                    histogram_min;				\
    int                    histogram_max;				\
    QList<QImage>          thumbnails;					\
    QImage                 defaultThumbnail; \
  };									\
  itkDataImage##suffix::itkDataImage##suffix(): dtkAbstractDataImage(), d (new itkDataImage##suffix##Private) \
  {									\
    d->image = 0;							\
    d->histogram = 0;							\
    d->range_computed = false;						\
    d->range_min = 0;							\
    d->range_max = 0;							\
    d->histogram_min = 0;						\
    d->histogram_max = 0;						\
    d->defaultThumbnail = QImage(128, 128, QImage::Format_ARGB32); \
    d->defaultThumbnail.fill(0); \
  }									\
  itkDataImage##suffix::~itkDataImage##suffix()				\
  {									\
    delete d;								\
    d = 0;								\
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
    if (image.IsNull()) {						\
      qDebug() << "Cannot cast data to correct data type";		\
      return;								\
    }									\
    d->image = image;							\
    d->range_computed = false;						\
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
    if (d->image.IsNull())						\
      return -1;							\
    return d->image->GetLargestPossibleRegion().GetSize()[0];		\
  }									\
  int itkDataImage##suffix::yDimension(void)				\
  {									\
    if (d->image.IsNull())						\
      return -1;							\
    return d->image->GetLargestPossibleRegion().GetSize()[1];		\
  }									\
  int itkDataImage##suffix::zDimension(void)				\
  {									\
    if (d->image.IsNull())						\
      return -1;							\
    return d->image->GetLargestPossibleRegion().GetSize()[2];		\
  }									\
                                                                        \
  void itkDataImage##suffix::computeRange()				\
  {									\
    if ( d->range_computed )						\
      return;								\
									\
    if ( d->image->GetLargestPossibleRegion().GetNumberOfPixels() == 0 ) \
      return;								\
									\
    typedef itkDataImage##suffix##Private::ImageType ImageType;		\
    typedef itk::MinimumMaximumImageCalculator<ImageType>		\
      MinMaxCalculatorType;						\
    MinMaxCalculatorType::Pointer calculator = MinMaxCalculatorType::New(); \
    calculator->SetImage ( d->image );					\
    calculator->SetRegion( d->image->GetLargestPossibleRegion() );	\
    try									\
    {									\
      calculator->Compute();						\
    }									\
    catch (itk::ExceptionObject &e)					\
    {									\
      std::cerr << e;							\
      return;								\
    }									\
    d->range_min = calculator->GetMinimum();				\
    d->range_max = calculator->GetMaximum();				\
    d->range_computed = true;						\
  }                                                                     \
                                                                        \
  int itkDataImage##suffix::tDimension(void)				\
  {									\
      if (d->image.IsNull())                                            \
          return -1;                                                    \
      if (dimension<4)                                                  \
          return 1;                                                     \
      else                                                              \
          return d->image->GetLargestPossibleRegion().GetSize()[3];     \
  }									\
                                                                        \
  int itkDataImage##suffix::minRangeValue(void)				\
  {									\
    computeRange();							\
    if ( !d->range_computed )						\
      qDebug() << "Cannot compute range";				\
    return d->range_min;						\
  }									\
  int itkDataImage##suffix::maxRangeValue(void)				\
  {									\
    computeRange();							\
    return d->range_max;						\
  }									\
  int itkDataImage##suffix::scalarValueCount(int value)			\
  {									\
    typedef itkDataImage##suffix##Private::ScalarType ScalarType;	\
									\
    computeValueCounts();						\
    if( (ScalarType)value>=d->range_min && (ScalarType)value<=d->range_max ) \
    {									\
      return d->histogram->GetFrequency( value, 0 );			\
    }									\
    return -1;								\
  }									\
  void itkDataImage##suffix::computeValueCounts()			\
  {									\
    if( d->histogram.IsNull() )						\
    {									\
      computeRange();							\
									\
      typedef itkDataImage##suffix##Private::HistogramGeneratorType	\
	HistogramGeneratorType;						\
      HistogramGeneratorType::Pointer histogramGenerator =		\
	HistogramGeneratorType::New();					\
      histogramGenerator->SetInput( d->image );				\
      histogramGenerator->SetNumberOfBins(d->range_max - d->range_min + 1); \
      histogramGenerator->SetMarginalScale( 1.0 );			\
      histogramGenerator->SetHistogramMin( d->range_min );		\
      histogramGenerator->SetHistogramMax( d->range_max );		\
      try								\
      {									\
	std::cerr << "calculating histogram...";			\
	histogramGenerator->Compute();					\
	std::cerr << "done" << std::endl;				\
      }									\
      catch (itk::ExceptionObject &e)					\
      {									\
	std::cerr << e;							\
	return;								\
      }									\
									\
      typedef HistogramGeneratorType::HistogramType  HistogramType;	\
      typedef HistogramType::AbsoluteFrequencyType   FrequencyType;	\
      d->histogram =							\
	const_cast<HistogramType*>( histogramGenerator->GetOutput() );	\
      FrequencyType min = itk::NumericTraits< FrequencyType >::max();	\
      FrequencyType max = itk::NumericTraits< FrequencyType >::min();	\
									\
      typedef HistogramType::Iterator Iterator;				\
      for ( Iterator it = d->histogram->Begin(), end = d->histogram->End(); \
	    it != end; ++it )						\
      {									\
	FrequencyType c = it.GetFrequency();				\
	if ( min > c ) min = c;						\
	if ( max < c ) max = c;						\
      }									\
      d->histogram_min = static_cast< int >( min );			\
      d->histogram_max = static_cast< int >( max );			\
    }									\
  }									\
  int itkDataImage##suffix::scalarValueMinCount(void)                   \
  {									\
    computeValueCounts();						\
    return d->histogram_min;						\
  }									\
  int itkDataImage##suffix::scalarValueMaxCount(void)			\
  {									\
    computeValueCounts();						\
    return d->histogram_max;						\
  }									\
  dtkAbstractData *createItkDataImage##suffix(void)			\
  {									\
    return new itkDataImage##suffix;					\
  }									\
									\
  QImage & itkDataImage##suffix::thumbnail (void) const			\
  {									\
  if (d->image.IsNull())						\
    return dtkAbstractDataImage::thumbnail();				\
									\
  int xydim = 128;							\
  if (d->thumbnails.isEmpty()            ||				\
      d->thumbnails[0].height() != xydim ||				\
      d->thumbnails[0].width()  != xydim )				\
    generateThumbnails<type, dimension>(d->image, xydim, true,		\
					d->thumbnails);			\
									\
  int index = 0;							\
  if (dimension > 2) {							\
	itkDataImage##suffix##Private::ImageType::SizeType size = d->image->GetLargestPossibleRegion().GetSize(); \
    index = size[2] / 2;					\
  } \
									\
  qDebug() << "thumbnail " << index << " / " << d->thumbnails.length(); \
  if (index < d->thumbnails.length())					\
    return d->thumbnails[index];					\
  else if (d->thumbnails.size()>0)									\
    return d->thumbnails[0];						\
  else \
	return d->defaultThumbnail; \
  }									\
									\
  QList<QImage> & itkDataImage##suffix::thumbnails (void) const		\
  {									\
    unsigned int xydim = 128;						\
    generateThumbnails<type, dimension>(d->image, xydim, false,		\
					d->thumbnails);			\
    return d->thumbnails;						\
  }									\
									\
  void itkDataImage##suffix::onMetaDataSet(const QString& key, const QString& value) {\
  Q_UNUSED(key);                                                        \
  Q_UNUSED(value);                                                      \
  }                                                                     \
  void itkDataImage##suffix::onPropertySet(const QString& key, const QString& value) {\
  Q_UNUSED(key);                                                        \
  Q_UNUSED(value);                                                      \
  }                                                                     \

#define medImplementITKVectorDataImage(type, dimension, suffix)		\
  class itkDataImage##suffix##Private					\
  {									\
  public:								\
    typedef type                                                          ScalarType; \
    typedef itk::Image<ScalarType, dimension>                             ImageType; \
  public:								\
    ImageType::Pointer          image;					\
    QList<QImage>               thumbnails;				\
  };									\
  itkDataImage##suffix::itkDataImage##suffix(): dtkAbstractDataImage(), d (new itkDataImage##suffix##Private) \
  {									\
    d->image = 0;							\
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
    if (image.IsNull()) {						\
      qDebug() << "Cannot cast data to correct data type";		\
      return;								\
    }									\
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
  int itkDataImage##suffix::tDimension(void)				\
  { 									\
    if (d->image.IsNull()) \
      return -1; \
    if (dimension<4) \
      return 1; \
    else \
      return d->image->GetLargestPossibleRegion().GetSize()[3];		\
  }									\
  int itkDataImage##suffix::minRangeValue(void)				\
  {									\
    return -1;								\
  }									\
  int itkDataImage##suffix::maxRangeValue(void)				\
  {									\
    return -1;								\
  }									\
  int itkDataImage##suffix::scalarValueCount(int value)			\
  {									\
    return -1;								\
  }									\
  int itkDataImage##suffix::scalarValueMinCount(void)                   \
  {									\
    return -1;								\
  }									\
  int itkDataImage##suffix::scalarValueMaxCount(void)			\
  {									\
    return -1;								\
  }									\
  dtkAbstractData *createItkDataImage##suffix(void)			\
  {									\
    return new itkDataImage##suffix;					\
  }									\
  QList<QImage> &itkDataImage##suffix::thumbnails (void) const		\
  {									\
  typedef itkDataImage##suffix##Private::ImageType ImageType;		\
  typedef itk::Image<type, 2>                      Image2DType;		\
  if (d->image.IsNull() )						\
    return d->thumbnails;						\
  if (ImageType::GetImageDimension()<2 )				\
    return d->thumbnails;						\
  ImageType::Pointer image = d->image;					\
  {									\
    ImageType::SizeType size = d->image->GetLargestPossibleRegion().GetSize(); \
    ImageType::SizeType newSize = size;					\
    newSize[0] = 128;							\
    newSize[1] = 128;							\
    unsigned int *sfactor = new unsigned int [ImageType::GetImageDimension()];		\
    for (unsigned int i=0; i<ImageType::GetImageDimension(); i++)	\
      sfactor[i] = size[i]/newSize[i];					\
    typedef itk::ShrinkImageFilter<ImageType, ImageType> FilterType;	\
    FilterType::Pointer filter = FilterType::New();			\
    filter->SetInput ( image );						\
    filter->SetShrinkFactors ( sfactor );				\
    try									\
    {									\
      filter->Update();							\
    }									\
    catch (itk::ExceptionObject &e)					\
    {									\
      qDebug() << e.GetDescription();					\
      return d->thumbnails;						\
    }									\
    image = filter->GetOutput();					\
	delete [] sfactor; \
  }									\
  ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize(); \
  itk::ImageRegionIterator<ImageType> it (image, image->GetLargestPossibleRegion()); \
  unsigned long nvoxels_per_slice = size[0]*size[1];			\
  unsigned long voxelCount = 0;						\
  QImage *qimage = new QImage (size[0], size[1], QImage::Format_ARGB32); \
  uchar *qImageBuffer = qimage->bits();					\
  while(!it.IsAtEnd())							\
  {									\
    *qImageBuffer++ = static_cast<unsigned char>(it.Value()[0]);	\
    *qImageBuffer++ = static_cast<unsigned char>(it.Value()[1]);	\
    *qImageBuffer++ = static_cast<unsigned char>(it.Value()[2]);	\
    *qImageBuffer++ = 0xFF;						\
    ++it;								\
    ++voxelCount;							\
    if ( (voxelCount%nvoxels_per_slice)==0 ) {				\
      d->thumbnails.push_back (*qimage);				\
      qimage = new QImage (size[0], size[1], QImage::Format_ARGB32);	\
      qImageBuffer = qimage->bits();					\
    }									\
  }									\
  return d->thumbnails;							\
  }									\
  void itkDataImage##suffix::onMetaDataSet(const QString& key, const QString& value) {\
  Q_UNUSED(key);                                                        \
  Q_UNUSED(value);                                                      \
  }                                                                     \
  void itkDataImage##suffix::onPropertySet(const QString& key, const QString& value) {\
  Q_UNUSED(key);                                                        \
  Q_UNUSED(value);                                                      \
  }                                                                     \
  
#endif


/*    
*/

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







/*  typedef itk::Image<RGBPixelType, 2>                           RGBImage2DType; \
  typedef itk::ExtractImageFilter<RGBImageType, RGBImage2DType> RGBExtractFilterType; \
  RGBExtractFilterType::Pointer extractor = RGBExtractFilterType::New(); \
  extractor->SetInput ( rgbfilter->GetOutput() );			\
  ImageType::SizeType size = rgbfilter->GetOutput()->GetLargestPossibleRegion().GetSize(); \
  for( unsigned int i=0; i<size[2]; i++ )				\
  {									\
    RGBImageType::RegionType region = rgbfilter->GetOutput()->GetLargestPossibleRegion(); \
    RGBImageType::IndexType index = region.GetIndex();			\
    RGBImageType::SizeType ssize = region.GetSize();			\
    index[2] = i;							\
    ssize[2] = 0;							\
    region.SetIndex ( index );						\
    region.SetSize ( ssize );						\
    extractor->SetExtractionRegion (region);				\
    try									\
    {									\
      extractor->Update();						\
    }									\
    catch (itk::ExceptionObject &e)					\
    {									\
      qDebug() << e.GetDescription();					\
      return d->thumbnails;						\
    }									\
    QImage qimage (ssize[0], ssize[1], QImage::Format_ARGB32);		\
    uchar *qImageBuffer = qimage.bits();				\
    itk::ImageRegionIterator<RGBImage2DType> it (extractor->GetOutput(), extractor->GetOutput()->GetLargestPossibleRegion()); \
    while(!it.IsAtEnd())						\
    {									\
    *qImageBuffer++ = it.Value().GetRed();				\
    *qImageBuffer++ = it.Value().GetGreen();				\
    *qImageBuffer++ = it.Value().GetBlue();				\
    *qImageBuffer++ = 0xFF;						\
    ++it;								\
    }									\
    d->thumbnails.push_back (qimage);					\
  }									\
*/



/*  double imMin = 0.0;							\
  double imMax = 0.0;							\
  {									\
    typedef itk::ExtractImageFilter<ImageType, Image2DType> ExtractFilterType; \
    ExtractFilterType::Pointer extractor = ExtractFilterType::New();	\
    extractor->SetInput ( image );					\
    ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize(); \
    ImageType::RegionType region = image->GetLargestPossibleRegion();	\
    ImageType::IndexType index = region.GetIndex();			\
    ImageType::SizeType ssize = region.GetSize();			\
    for( unsigned int i=2; i<ImageType::GetImageDimension(); i++) {		\
      index[i] /= 2;							\
      ssize[i]  = 0;							\
    }									\
    region.SetIndex ( index );						\
    region.SetSize ( ssize );						\
    extractor->SetExtractionRegion (region);				\
    try									\
    {									\
      extractor->Update();						\
    }									\
    catch (itk::ExceptionObject &e)					\
    {									\
      qDebug() << e.GetDescription();					\
      return d->thumbnails;						\
    }									\
    typedef itk::MinimumMaximumImageCalculator<Image2DType> MinMaxCalculatorType; \
    MinMaxCalculatorType::Pointer calculator = MinMaxCalculatorType::New(); \
    calculator->SetImage ( extractor->GetOutput() );			\
    try									\
    {									\
      calculator->Compute();						\
    }									\
    catch (itk::ExceptionObject &e)					\
    {									\
      qDebug() << e.GetDescription();					\
      return d->thumbnails;						\
    }									\
    imMin = calculator->GetMinimum();					\
    imMax = calculator->GetMaximum();					\
    typedef itk::Statistics::ScalarImageToHistogramGenerator< Image2DType > HistogramGeneratorType; \
    HistogramGeneratorType::Pointer histogramGenerator = HistogramGeneratorType::New();	\
    histogramGenerator->SetInput( extractor->GetOutput() );		\
    histogramGenerator->SetNumberOfBins( imMax - imMin + 1 );		\
    histogramGenerator->SetMarginalScale( 1.0 );			\
    histogramGenerator->SetHistogramMin( imMin );			\
    histogramGenerator->SetHistogramMax( imMax );			\
    try									\
    {									\
      histogramGenerator->Compute();					\
    }									\
    catch (itk::ExceptionObject &e)					\
    {									\
      qDebug() << e.GetDescription();					\
      return d->thumbnails;						\
    }									\
    typedef HistogramGeneratorType::HistogramType  HistogramType;	\
    HistogramType::Pointer histogram = const_cast<HistogramType*>( histogramGenerator->GetOutput() ); \
    double totalFreq = histogram->GetTotalFrequency();			\
    int ind_min = 0;							\
    int ind_max = histogram->Size()-1;					\
    double freq_min = histogram->GetFrequency (ind_min)/totalFreq;	\
    double freq_max = histogram->GetFrequency (ind_max)/totalFreq;	\
    while ( freq_min<0.01 ) {						\
      ind_min++;							\
      imMin++;								\
      freq_min += histogram->GetFrequency (ind_min) / totalFreq;	\
    }									\
    while ( freq_max<0.01 ) {						\
      ind_max--;							\
      imMax--;								\
      freq_max += histogram->GetFrequency (ind_max) / totalFreq;	\
    }									\
  }									\
*/



/*  rgbfilter->GetColormap()->SetMinimumInputValue ( imMin );		\
  rgbfilter->GetColormap()->SetMaximumInputValue ( imMax );		\
*/


/*    typedef itk::ShrinkImageFilter<FloatImageType, ImageType> FilterType; \
    FilterType::Pointer filter = FilterType::New();			\
    filter->SetInput ( smoother->GetOutput() );				\
    filter->SetShrinkFactors ( sfactor );				\
    try									\
    {									\
      filter->Update();							\
    }									\
*/
