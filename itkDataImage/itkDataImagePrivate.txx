#ifndef _itkDataImagePrivate_txx_
#define _itkDataImagePrivate_txx_

template < typename TPixel, unsigned int VDimension>
itkDataImagePrivate< TPixel , VDimension >::itkDataImagePrivate( itkDataImageBase * _self ) :
self( _self ),
    m_image (0),
    m_range_computed( false ),
    m_range_min( 0 ),
    m_range_max( 0 ),
    m_histogram (0),
    m_histogram_min( 0 ),
    m_histogram_max( 0 ),
    m_defaultThumbnail( QImage(128, 128, QImage::Format_ARGB32) ) 
{
    m_defaultThumbnail.fill(0);
}

template < typename TPixel, unsigned int VDimension>
itkDataImagePrivate< TPixel , VDimension >::~itkDataImagePrivate()
{
}

template < typename TPixel, unsigned int VDimension>
void itkDataImagePrivate< TPixel , VDimension >::setData(void *data)
{
    typename ImageType::Pointer image = dynamic_cast<ImageType*>( (itk::Object*) data );
    if (image.IsNull()) {
        qDebug() << "Cannot cast data to correct data TPixel";
        return;
    }
    m_image = image;
    m_range_computed = false;
}

template < typename TPixel, unsigned int VDimension>
void *itkDataImagePrivate< TPixel , VDimension >::output(void)
{
    return m_image.GetPointer();
}

template < typename TPixel, unsigned int VDimension>
void *itkDataImagePrivate< TPixel , VDimension >::data(void)
{
    return m_image.GetPointer();
}

template < typename TPixel, unsigned int VDimension>
void itkDataImagePrivate< TPixel , VDimension >::update(void)
{
}

template < typename TPixel, unsigned int VDimension>
int itkDataImagePrivate< TPixel , VDimension >::xDimension(void)
{
    if (m_image.IsNull())
        return -1;
    return m_image->GetLargestPossibleRegion().GetSize()[0];
}

template < typename TPixel, unsigned int VDimension>
int itkDataImagePrivate< TPixel , VDimension >::yDimension(void)
{
    if (m_image.IsNull())
        return -1;
    return m_image->GetLargestPossibleRegion().GetSize()[1];
}

template < typename TPixel, unsigned int VDimension>
int itkDataImagePrivate< TPixel , VDimension >::zDimension(void)
{
    if (m_image.IsNull())
        return -1;
    return m_image->GetLargestPossibleRegion().GetSize()[2];
}

template < typename TPixel, unsigned int VDimension>
void itkDataImagePrivate< TPixel , VDimension >::computeRange()
{
    if ( m_range_computed )
        return;

    if ( m_image->GetLargestPossibleRegion().GetNumberOfPixels() == 0 )
        return;

    typedef itkDataImagePrivate::ImageType ImageType;
    typedef itk::MinimumMaximumImageCalculator<ImageType>
        MinMaxCalculatorType;
    typename MinMaxCalculatorType::Pointer calculator = MinMaxCalculatorType::New();
    calculator->SetImage ( m_image );
    calculator->SetRegion( m_image->GetLargestPossibleRegion() );
    try
    {
        calculator->Compute();
    }
    catch (itk::ExceptionObject &e)
    {
        std::cerr << e;
        return;
    }
    m_range_min = calculator->GetMinimum();
    m_range_max = calculator->GetMaximum();
    m_range_computed = true;
}

template < typename TPixel, unsigned int VDimension>
int itkDataImagePrivate< TPixel , VDimension >::tDimension(void)
{
    if (m_image.IsNull())
        return -1;
    if (VDimension<4)
        return 1;
    else
        return m_image->GetLargestPossibleRegion().GetSize()[3];
}

template < typename TPixel, unsigned int VDimension>
int itkDataImagePrivate< TPixel , VDimension >::minRangeValue(void)
{
    computeRange();
    if ( !m_range_computed )
        qDebug() << "Cannot compute range";
    return m_range_min;
}

template < typename TPixel , unsigned int VDimension >
int itkDataImagePrivate< TPixel , VDimension >::maxRangeValue(void)
{
    computeRange();
    return m_range_max;
}

template < typename TPixel , unsigned int VDimension >
int itkDataImagePrivate< TPixel , VDimension >::scalarValueCount(int value)
{
    typedef itkDataImagePrivate::ScalarType ScalarType;

    computeValueCounts();
    if( (ScalarType)value>=m_range_min && (ScalarType)value<=m_range_max )
    {
        return m_histogram->GetFrequency( value, 0 );
    }
    return -1;
}

template < typename TPixel , unsigned int VDimension >
void itkDataImagePrivate< TPixel , VDimension >::computeValueCounts()
{
    if( m_histogram.IsNull() )
    {
        computeRange();

        typedef itkDataImagePrivate::HistogramGeneratorType
            HistogramGeneratorType;
        typename HistogramGeneratorType::Pointer histogramGenerator =
            HistogramGeneratorType::New();
        histogramGenerator->SetInput( m_image );
        histogramGenerator->SetNumberOfBins(m_range_max - m_range_min + 1);
        histogramGenerator->SetMarginalScale( 1.0 );
        histogramGenerator->SetHistogramMin( m_range_min );
        histogramGenerator->SetHistogramMax( m_range_max );
        try
        {
            std::cerr << "calculating histogram...";
            histogramGenerator->Compute();
            std::cerr << "done" << std::endl;
        }
        catch (itk::ExceptionObject &e)
        {
            std::cerr << e;
            return;
        }

        typedef typename HistogramGeneratorType::HistogramType  HistogramType;
        typedef typename HistogramType::AbsoluteFrequencyType   FrequencyType;
        m_histogram =
            const_cast<HistogramType*>( histogramGenerator->GetOutput() );
        FrequencyType min = itk::NumericTraits< FrequencyType >::max();
        FrequencyType max = itk::NumericTraits< FrequencyType >::min();

        typedef typename HistogramType::Iterator Iterator;
        for ( Iterator it = m_histogram->Begin(), end = m_histogram->End();
            it != end; ++it )
        {
            FrequencyType c = it.GetFrequency();
            if ( min > c ) min = c;
            if ( max < c ) max = c;
        }
        m_histogram_min = static_cast< int >( min );
        m_histogram_max = static_cast< int >( max );
    }
}

template < typename TPixel, unsigned int VDimension>
int itkDataImagePrivate< TPixel , VDimension >::scalarValueMinCount(void)
{
    computeValueCounts();
    return m_histogram_min;
}

template < typename TPixel, unsigned int VDimension>
int itkDataImagePrivate< TPixel , VDimension >::scalarValueMaxCount(void)
{
    computeValueCounts();
    return m_histogram_max;
}

template < typename TPixel, unsigned int VDimension>
QImage & itkDataImagePrivate< TPixel , VDimension >::thumbnail (void)
{
    if (m_image.IsNull())
        return self->dtkAbstractDataImage::thumbnail();

    int xydim = 128;
    if (m_thumbnails.isEmpty()            ||
        m_thumbnails[0].height() != xydim ||
        m_thumbnails[0].width()  != xydim )
        generateThumbnails( m_image, xydim, true, m_thumbnails);

    int index = 0;
    if (VDimension > 2) {
        typename itkDataImagePrivate::ImageType::SizeType size = m_image->GetLargestPossibleRegion().GetSize();
        index = size[2] / 2;
    }


    if (index < m_thumbnails.length())
        return m_thumbnails[index];
    else if (m_thumbnails.size()>0)
        return m_thumbnails[0];
    else
        return m_defaultThumbnail;
}


template < typename TPixel, unsigned int VDimension>
QList<QImage> & itkDataImagePrivate< TPixel , VDimension >::thumbnails (void)
{
    unsigned int xydim = 128;
    generateThumbnails( m_image, xydim, false, m_thumbnails);
    return m_thumbnails;
}

//static
template< typename TPixel, unsigned int VDimension >
void itkDataImagePrivate< TPixel , VDimension >::generateThumbnails (typename itk::Image<TPixel, VDimension>::Pointer image,
    int xydim, bool singlez, QList<QImage> & thumbnails)
{
    if (VDimension < 3)
        return;
    if (image.IsNull())
        return;

    typedef typename itk::Image<TPixel, VDimension> ImageType;
    typename ImageType::Pointer  img  = image;

    img->SetRequestedRegion(img->GetLargestPossibleRegion());
    img->Update();
    typename ImageType::SizeType size = img->GetLargestPossibleRegion().GetSize();

    if ((( singlez && thumbnails.length() == 1) ||
        (!singlez && thumbnails.length() == static_cast< int >( size[2] ))) &&
        thumbnails.length()    >  1     &&
        thumbnails[0].height() == xydim &&
        thumbnails[0].width()  == xydim)
        return;

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

        typedef typename itk::RegionOfInterestImageFilter<ImageType, ImageType> FilterType;
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

        typedef typename itk::Image<float, VDimension> FloatImageType;
        typedef typename itk::RecursiveGaussianImageFilter<ImageType, FloatImageType>
            SmootherType0;
        typedef typename itk::RecursiveGaussianImageFilter<FloatImageType, FloatImageType>
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

        typedef typename itk::ResampleImageFilter<FloatImageType, ImageType> FilterType;
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

    typedef typename itk::RGBPixel<unsigned char> RGBPixelType;
    typedef typename itk::Image<RGBPixelType, VDimension> RGBImageType;
    typedef typename itk::ScalarToRGBColormapImageFilter<ImageType, RGBImageType> RGBFilterType;
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
    QImage *qimage = new QImage (size[0], size[1], QImage::Format_ARGB32);
    uchar	 *qImageBuffer = qimage->bits();
    itk::ImageRegionIterator<RGBImageType> it (rgbfilter->GetOutput(),
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
            thumbnails.push_back (qimage->mirrored(img->GetDirection()(0,0) == -1.0, img->GetDirection()(1,1) == -1.0));
            delete qimage;
            qimage = new QImage (size[0], size[1], QImage::Format_ARGB32);
            qImageBuffer = qimage->bits();
        }
    }

    delete qimage;

}

#endif // _itkDataImagePrivate_txx_
