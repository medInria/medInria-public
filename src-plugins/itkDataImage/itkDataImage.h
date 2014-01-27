/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkVector.h>
#include <itkImage.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkScalarImageToHistogramGenerator.h>
#include <itkRGBPixel.h>
#include <itkRGBAPixel.h>
#include <itkScalarToRGBColormapImageFilter.h>
#include <itkRegionOfInterestImageFilter.h>
#include <itkExtractImageFilter.h>
#include <itkShrinkImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkRecursiveGaussianImageFilter.h>

#include <medAbstractDataFactory.h>
#include <medAbstractDataTypedImage.h>
#include <itkDataImagePluginExport.h>

template<typename T,int DIM>
std::vector <bool> DeterminePermutationsAndFlips(typename itk::Image<T,DIM>::DirectionType &directionMatrix)
{
    std::vector <bool> mirrorThumbs(2,false);
    std::vector <unsigned int> axesPermutation(3,false);
    std::vector <bool> maxValueNegative(3,false);

    // Determine permutations, used to see if we have axial, sagittal or coronal main direction (z-direction)
    for (unsigned int i = 0;i < 3;++i)
    {
        axesPermutation[i] = i;
        double maxAbsValue = directionMatrix(i,i);

        for (unsigned int j = 0;j < DIM;++j)
        {
            if (fabs(maxAbsValue) < fabs(directionMatrix(j,i)))
            {
                axesPermutation[i] = j;
                maxAbsValue = directionMatrix(j,i);
            }
        }
        if (maxAbsValue < 0)
            maxValueNegative[i] = true;
    }

    // Modify flips according to main orientation and directions, as well as QImage inversion of y axis
    if ((axesPermutation[2] > axesPermutation[0])&&(axesPermutation[2] > axesPermutation[1]))
    {
        // Axial
        qDebug() << "Axial first";
        mirrorThumbs[0] = maxValueNegative[0];
        mirrorThumbs[1] = maxValueNegative[1];
    }
    else if ((axesPermutation[2] < axesPermutation[0])&&(axesPermutation[2] < axesPermutation[1]))
    {
        // Sagittal
        qDebug() << "Sagittal first";
        mirrorThumbs[0] = !maxValueNegative[0];
        mirrorThumbs[1] = !maxValueNegative[1];
    }
    else
    {
        // Coronal
        qDebug() << "Coronal first";
        mirrorThumbs[0] = !maxValueNegative[0];
        mirrorThumbs[1] = !maxValueNegative[1];
    }

    return mirrorThumbs;
}


template<typename T,int DIM>
void generateThumbnails(typename itk::Image<T,DIM>* image,int xydim,bool singlez,QList<QImage>& thumbnails) {
    if (DIM<3 || !image)
        return;

    typedef itk::Image<T,DIM> ImageType;
    typename ImageType::Pointer  img  = image;

    img->SetRequestedRegion(img->GetLargestPossibleRegion());
    img->Update();
    typename ImageType::SizeType size = img->GetLargestPossibleRegion().GetSize();

    if ((( singlez && thumbnails.length() == 1) || (!singlez && thumbnails.length() == static_cast< int >( size[2] ))) &&
        thumbnails.length()    >  1     &&
        thumbnails[0].height() == xydim &&
        thumbnails[0].width()  == xydim)
    return;

    if (singlez) {
        typename ImageType::SizeType  newSize = size;
        typename ImageType::IndexType index;

        newSize[2] = 1;

        if (DIM==4)
            newSize[3] = 1;

        index.Fill(0);
        index[2] = size[2]/2;
        typename ImageType::RegionType region = img->GetLargestPossibleRegion();
        region.SetIndex(index);
        region.SetSize(newSize);

        typedef itk::RegionOfInterestImageFilter<ImageType,ImageType> FilterType;
        typename FilterType::Pointer extractor = FilterType::New();
        extractor->SetInput(img);
        extractor->SetRegionOfInterest(region);
        try {
            extractor->Update();
        } catch (itk::ExceptionObject& e) {
            qDebug() << e.GetDescription();
            return;
        }
        img = extractor->GetOutput();
        size = img->GetLargestPossibleRegion().GetSize();
        img->DisconnectPipeline();
    }

    thumbnails.clear();

    typedef itk::Image<T,2>     Image2DType;
    typedef itk::Image<float,2> FloatImage2DType;

    typedef itk::ExtractImageFilter<ImageType,Image2DType> ExtractFilterType;
    typename ExtractFilterType::Pointer extractor = ExtractFilterType::New();

    typename ImageType::RegionType extractionRegion = img->GetLargestPossibleRegion();
    extractionRegion.SetIndex(2,0);
    extractionRegion.SetSize (2,0);

    if (DIM==4) {
        extractionRegion.SetIndex(3,0);
        extractionRegion.SetSize (3,0);
    }

    extractor->SetExtractionRegion(extractionRegion);
    extractor->SetInput(img);
    extractor->SetDirectionCollapseToGuess();

    try {
        extractor->UpdateOutputInformation();
    } catch (itk::ExceptionObject& e) {
        qDebug() << e.GetDescription();
        return;
    }
    
    // setup resampling pipeline

    typename Image2DType::SpacingType spacing    = extractor->GetOutput()->GetSpacing();
    typename Image2DType::SpacingType newSpacing = spacing;
    typename Image2DType::SizeType    newSize    = extractor->GetOutput()->GetLargestPossibleRegion().GetSize();
    newSize[0] = xydim;
    newSize[1] = xydim;

    typename Image2DType::SpacingType sfactor, sigma, variance;

    for (unsigned int i=0;i<2;++i) {
        sfactor[i]     = (double)size[i]/(double)newSize[i];
        newSpacing[i] *= sfactor[i];
        sigma[i]       = 0.5 * sfactor[i];
    }

    int index	       = size[0] > size[1] ? 0 : 1;
    sfactor[!index]    = sfactor[index];
    variance[!index]   = variance[index];
    newSpacing[!index] = newSpacing[index];

    typename Image2DType::PointType origin = extractor->GetOutput()->GetOrigin();
    origin[!index] -= 0.5*(newSize[!index]*newSpacing[!index]-size[!index]*spacing[!index]);

    typedef itk::RecursiveGaussianImageFilter<Image2DType,FloatImage2DType>      SmootherType0;
    typedef itk::RecursiveGaussianImageFilter<FloatImage2DType,FloatImage2DType> SmootherType1;
    typename SmootherType0::Pointer smoother0 = SmootherType0::New();
    typename SmootherType1::Pointer smoother1 = SmootherType1::New();
    smoother0->SetSigma(sigma[0]*spacing[0]);
    smoother1->SetSigma(sigma[1]*spacing[1]);
    smoother0->SetDirection(0);
    smoother1->SetDirection(1);
    smoother0->SetOrder(SmootherType0::ZeroOrder);
    smoother1->SetOrder(SmootherType1::ZeroOrder);
    //smoother0->SetInput(extractor->GetOutput());
    smoother1->SetInput(smoother0->GetOutput());
    typename SmootherType1::Pointer smoother = smoother1;

    typedef typename itk::ResampleImageFilter<FloatImage2DType,Image2DType> FilterType;
    typename FilterType::Pointer resampler = FilterType::New();
    resampler->SetInput(smoother->GetOutput());
    resampler->SetSize(newSize);
    resampler->SetOutputSpacing(newSpacing);
    resampler->SetOutputOrigin(origin);
    resampler->SetOutputDirection(extractor->GetOutput()->GetDirection());

    // setup color mapping

    typedef itk::RGBPixel<unsigned char> RGBPixelType;
    typedef itk::Image<RGBPixelType, 2>  RGBImage2DType;
    typedef itk::ScalarToRGBColormapImageFilter<Image2DType, RGBImage2DType> RGBFilterType;
    typename RGBFilterType::Pointer rgbfilter = RGBFilterType::New();
    rgbfilter->SetColormap (RGBFilterType::Grey);
    rgbfilter->GetColormap()->SetMinimumRGBComponentValue (0);
    rgbfilter->GetColormap()->SetMaximumRGBComponentValue (255);
    rgbfilter->UseInputImageExtremaForScalingOn ();

    typename ImageType::DirectionType directionMatrix = image->GetDirection();

    std::vector <bool> mirrorThumbs = DeterminePermutationsAndFlips<T,DIM>(directionMatrix);

    if (DIM==3) {

        for (unsigned int slice=0;slice<size[2];slice++) {

            extractionRegion.SetIndex(2, slice);
            extractor->SetExtractionRegion (extractionRegion);

            extractor->Update();

            typename Image2DType::Pointer img2d = extractor->GetOutput();

            if (size[0] > static_cast<unsigned int>(xydim) ||
                size[1] > static_cast<unsigned int>(xydim) ) {

                smoother0->SetInput(extractor->GetOutput());
                smoother0->Modified();

                try {
                    resampler->Update();
                } catch (itk::ExceptionObject &e) {
                    qDebug() << e.GetDescription();
                    return;
                }

                img2d = resampler->GetOutput();
                img2d->DisconnectPipeline();
            }

            rgbfilter->SetInput (img2d);
            rgbfilter->Modified();

            try {
                rgbfilter->Update();
            } catch (itk::ExceptionObject &e) {
                qDebug() << e.GetDescription();
                return;
            }

            QImage *qimage = new QImage (newSize[0],newSize[1],QImage::Format_ARGB32);
            qimage->fill( QColor::fromRgbF(0,0,0).rgba());
            uchar  *qImageBuffer = qimage->bits();

            unsigned int baseX = 0;
            unsigned int baseY = 0;

            if (newSize[0] > rgbfilter->GetOutput()->GetLargestPossibleRegion().GetSize()[0])
                baseX = (newSize[0] - rgbfilter->GetOutput()->GetLargestPossibleRegion().GetSize()[0]) / 2;
            if (newSize[1] > rgbfilter->GetOutput()->GetLargestPossibleRegion().GetSize()[1])
                baseY = (newSize[1] - rgbfilter->GetOutput()->GetLargestPossibleRegion().GetSize()[1]) / 2;

            itk::ImageRegionIteratorWithIndex<RGBImage2DType> it (rgbfilter->GetOutput(),
                                                         rgbfilter->GetOutput()->GetLargestPossibleRegion());

            while (!it.IsAtEnd()) {
                RGBImage2DType::IndexType tmpIndex = it.GetIndex();
                qImageBuffer[4 * ((baseY + tmpIndex[1]) * newSize[0] + baseX + tmpIndex[0])] = static_cast<unsigned char>(it.Value().GetRed());
                qImageBuffer[4 * ((baseY + tmpIndex[1]) * newSize[0] + baseX + tmpIndex[0]) + 1] = static_cast<unsigned char>(it.Value().GetGreen());
                qImageBuffer[4 * ((baseY + tmpIndex[1]) * newSize[0] + baseX + tmpIndex[0]) + 2] = static_cast<unsigned char>(it.Value().GetBlue());
                qImageBuffer[4 * ((baseY + tmpIndex[1]) * newSize[0] + baseX + tmpIndex[0]) + 3] = 0xFF;

                ++it;
            }

            thumbnails.push_back(qimage->mirrored(mirrorThumbs[0],mirrorThumbs[1]));
            delete qimage;
        }
    } else if (DIM==4) {
        for (unsigned int volume=0; volume<size[3]; volume++) {
            for (unsigned int slice=0; slice<size[2]; slice++) {

                extractionRegion.SetIndex(2, slice);
                extractionRegion.SetIndex(3, volume);
                extractor->SetExtractionRegion (extractionRegion);

                extractor->Update();

                typename Image2DType::Pointer img2d = extractor->GetOutput();

                if (size[0] > static_cast<unsigned int>(xydim) ||
                    size[1] > static_cast<unsigned int>(xydim) ) {

                    smoother0->SetInput( extractor->GetOutput() );
                    smoother0->Modified();

                    try {
                        resampler->Update();
                    } catch (itk::ExceptionObject &e) {
                        qDebug() << e.GetDescription();
                        return;
                    }

                    img2d = resampler->GetOutput();
                    img2d->DisconnectPipeline();
                }

                rgbfilter->SetInput (img2d);
                rgbfilter->Modified();

                try {
                    rgbfilter->Update();
                } catch (itk::ExceptionObject &e) {
                    qDebug() << e.GetDescription();
                    return;
                }

                // qDebug() << "Time elapsed: " << time.elapsed();

                QImage *qimage = new QImage (newSize[0],newSize[1],QImage::Format_ARGB32);
                qimage->fill(QColor::fromRgbF(0,0,0).rgba());
                uchar  *qImageBuffer = qimage->bits();

                unsigned int baseX = 0;
                unsigned int baseY = 0;

                if (newSize[0] > rgbfilter->GetOutput()->GetLargestPossibleRegion().GetSize()[0])
                    baseX = (newSize[0] - rgbfilter->GetOutput()->GetLargestPossibleRegion().GetSize()[0]) / 2;
                if (newSize[1] > rgbfilter->GetOutput()->GetLargestPossibleRegion().GetSize()[1])
                    baseY = (newSize[1] - rgbfilter->GetOutput()->GetLargestPossibleRegion().GetSize()[1]) / 2;

                itk::ImageRegionIteratorWithIndex<RGBImage2DType> it (rgbfilter->GetOutput(),
                                                                      rgbfilter->GetOutput()->GetLargestPossibleRegion());

                while (!it.IsAtEnd()) {
                    RGBImage2DType::IndexType tmpIndex = it.GetIndex();
                    qImageBuffer[4 * ((baseY + tmpIndex[1]) * newSize[0] + baseX + tmpIndex[0])] = static_cast<unsigned char>(it.Value().GetRed());
                    qImageBuffer[4 * ((baseY + tmpIndex[1]) * newSize[0] + baseX + tmpIndex[0]) + 1] = static_cast<unsigned char>(it.Value().GetGreen());
                    qImageBuffer[4 * ((baseY + tmpIndex[1]) * newSize[0] + baseX + tmpIndex[0]) + 2] = static_cast<unsigned char>(it.Value().GetBlue());
                    qImageBuffer[4 * ((baseY + tmpIndex[1]) * newSize[0] + baseX + tmpIndex[0]) + 3] = 0xFF;

                    ++it;
                }

                thumbnails.push_back(qimage->mirrored(mirrorThumbs[0],mirrorThumbs[1]));
                delete qimage;
            }
        }
    }
}

template <typename T>
struct ImageTypeIndex {
    static const unsigned Index = 0;
};

template <typename T,unsigned N>
struct ImageTypeIndex<itk::Vector<T,N> > {
    static const unsigned Index = 1;
};

template <typename T>
struct ImageTypeIndex<itk::RGBPixel<T> > {
    static const unsigned Index = 1;
};

template <typename T>
struct ImageTypeIndex<itk::RGBAPixel<T> > {
    static const unsigned Index = 1;
};

template <unsigned DIM,typename T,unsigned N>
class itkDataImagePrivate {
public:
    typedef T                                                                    ScalarType;
    typedef typename itk::Image<ScalarType,DIM>                                  ImageType;
    typedef typename itk::Statistics::ScalarImageToHistogramGenerator<ImageType> HistogramGeneratorType;
    typedef typename HistogramGeneratorType::HistogramType                       HistogramType;

    itkDataImagePrivate() {
        image = 0;
        histogram = 0;
        range_computed = false;
        range_min = 0;
        range_max = 0;
        histogram_min = 0;
        histogram_max = 0;
    }

    void reset() { range_computed = false; }

    int minRangeValue() {
        computeRange();
        if (!range_computed)
            qDebug() << "Cannot compute range";
        return range_min;
    }

    int maxRangeValue() {
        computeRange();
        return range_max;
    }

    int scalarValueCount(int value) {
        computeValueCounts();
        if((ScalarType)value>=range_min && (ScalarType)value<=range_max)
            return histogram->GetFrequency(value,0);
        return -1;
    }

    int scalarValueMinCount() {
        computeValueCounts();
        return histogram_min;
    }

    int scalarValueMaxCount() {
        computeValueCounts();
        return histogram_max;
    }

    QList<QImage>& make_thumbnails(const int sz,const bool singlez) {
        generateThumbnails<T,DIM>(image,sz,singlez,thumbnails);
        return thumbnails;
    }

    typename ImageType::Pointer     image;
    bool                            range_computed;
    ScalarType                      range_min;
    ScalarType                      range_max;
    typename HistogramType::Pointer histogram;
    int                             histogram_min;
    int                             histogram_max;
    QList<QImage>                   thumbnails;

private:

    void computeRange();
    void computeValueCounts();
};

template <unsigned DIM,typename T,unsigned N>
void itkDataImagePrivate<DIM,T,N>::computeRange() {
    if (range_computed)
        return;

    if (image->GetLargestPossibleRegion().GetNumberOfPixels()==0)
        return;

    typedef itk::MinimumMaximumImageCalculator<ImageType>  MinMaxCalculatorType;

    typename MinMaxCalculatorType::Pointer calculator = MinMaxCalculatorType::New();
    calculator->SetImage (image);
    calculator->SetRegion(image->GetLargestPossibleRegion());
    try {
        calculator->Compute();
    } catch (itk::ExceptionObject &e) {
        std::cerr << e;
        return;
    }
    range_min = calculator->GetMinimum();
    range_max = calculator->GetMaximum();
    range_computed = true;
}

template <unsigned DIM,typename T,unsigned N>
void itkDataImagePrivate<DIM,T,N>::computeValueCounts() {
    if (histogram.IsNull()) {
        computeRange();

        typename HistogramGeneratorType::Pointer histogramGenerator = HistogramGeneratorType::New();
        histogramGenerator->SetInput(image);
        histogramGenerator->SetNumberOfBins(range_max-range_min+1);
        histogramGenerator->SetMarginalScale(1.0);
        histogramGenerator->SetHistogramMin(range_min);
        histogramGenerator->SetHistogramMax(range_max);

        try {
            std::cerr << "calculating histogram...";
            histogramGenerator->Compute();
            std::cerr << "done" << std::endl;
        } catch (itk::ExceptionObject &e) {
            std::cerr << e;
            return;
        }

        typedef typename HistogramGeneratorType::HistogramType HistogramType;
        typedef typename HistogramType::AbsoluteFrequencyType  FrequencyType;

        histogram = const_cast<HistogramType*>(histogramGenerator->GetOutput());

        FrequencyType min = itk::NumericTraits< FrequencyType >::max();
        FrequencyType max = itk::NumericTraits< FrequencyType >::min();

        typedef typename HistogramType::Iterator Iterator;
        for (Iterator it=histogram->Begin(),end=histogram->End();it!=end;++it) {
            FrequencyType c = it.GetFrequency();
            if (min>c) min = c;
            if (max<c) max = c;
        }
        histogram_min = static_cast<int>(min);
        histogram_max = static_cast<int>(max);
    }
}

template <unsigned DIM,typename T>
class itkDataImagePrivate<DIM,T,1> {
public:
    typedef T                                   ScalarType;
    typedef typename itk::Image<ScalarType,DIM> ImageType;

    itkDataImagePrivate() { image = 0; }

    void reset() const { }

    int minRangeValue() const { return -1; }
    int maxRangeValue() const { return -1; }

    int scalarValueCount(int) const { return -1; }
    int scalarValueMinCount() const { return -1; }
    int scalarValueMaxCount() const { return -1; }

    QList<QImage>& make_thumbnails(const int sz,const bool singlez);

    typename ImageType::Pointer image;
    QList<QImage>               thumbnails;
};

template <unsigned DIM,typename T>
QList<QImage>& itkDataImagePrivate<DIM,T,1>::make_thumbnails(const int sz,const bool singlez) {
    typename ImageType::Pointer im = image;
    typename ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize();
    typename ImageType::SizeType newSize = size;
    newSize[0] = 128;
    newSize[1] = 128;
    unsigned int *sfactor = new unsigned int [ImageType::GetImageDimension()];
    for (unsigned int i=0; i<ImageType::GetImageDimension(); i++)
        sfactor[i] = ceil((float)size[i]/newSize[i]);
    typedef itk::ShrinkImageFilter<ImageType,ImageType> FilterType;
    typename FilterType::Pointer filter = FilterType::New();
    filter->SetInput(im);
    filter->SetShrinkFactors(sfactor);
    try {
        filter->Update();
    } catch (itk::ExceptionObject &e) {
        qDebug() << e.GetDescription();
        return thumbnails;
    }
    
    im = filter->GetOutput();
    delete [] sfactor;
    
    size = im->GetLargestPossibleRegion().GetSize();
    itk::ImageRegionIterator<ImageType> it (im,im->GetLargestPossibleRegion());

    unsigned int baseX = 0;
    unsigned int baseY = 0;

    if (newSize[0] > size[0])
        baseX = (newSize[0] - size[0]) / 2;
    if (newSize[1] > size[1])
        baseY = (newSize[1] - size[1]) / 2;

    unsigned long nvoxels_per_slice = size[0]*size[1];
    unsigned long voxelCount = 0;
    QImage *qimage = new QImage (newSize[0],newSize[1],QImage::Format_ARGB32);
    qimage->fill(QColor::fromRgbF(0,0,0).rgba());
    uchar *qImageBuffer = qimage->bits();
    while(!it.IsAtEnd()) {
        typename ImageType::IndexType tmpIndex = it.GetIndex();
        qImageBuffer[4 * ((baseY + tmpIndex[1]) * newSize[0] + baseX + tmpIndex[0])] = static_cast<unsigned char>(it.Value()[0]);
        qImageBuffer[4 * ((baseY + tmpIndex[1]) * newSize[0] + baseX + tmpIndex[0]) + 1] = static_cast<unsigned char>(it.Value()[1]);
        qImageBuffer[4 * ((baseY + tmpIndex[1]) * newSize[0] + baseX + tmpIndex[0]) + 2] = static_cast<unsigned char>(it.Value()[2]);
        qImageBuffer[4 * ((baseY + tmpIndex[1]) * newSize[0] + baseX + tmpIndex[0]) + 3] = 0xFF;

        ++it;
        ++voxelCount;
        if ((voxelCount%nvoxels_per_slice)==0) {
            thumbnails.push_back (*qimage);
            qimage = new QImage (newSize[0],newSize[1],QImage::Format_ARGB32);
            qimage->fill(QColor::fromRgbF(0,0,0).rgba());
            qImageBuffer = qimage->bits();
        }
    }

    return thumbnails;
}

template <unsigned DIM,typename T,const char* ID> medAbstractData* createItkDataImage();

template <unsigned DIM,typename T,const char* ID>
class ITKDATAIMAGEPLUGIN_EXPORT itkDataImage: public medAbstractDataTypedImage<DIM,T> {

    typedef itkDataImagePrivate<DIM,T,ImageTypeIndex<T>::Index> PrivateMember;

public:

    itkDataImage(): medAbstractDataTypedImage<DIM,T>(),d(new PrivateMember) { }

    ~itkDataImage() {
        delete d;
        d = 0;
    }

    virtual QString description() const { return "itk image data" ; }
    virtual QString identifier() const { return ID; }
    static bool registered() {
        return medAbstractDataFactory::instance()->registerDataType(ID,createItkDataImage<DIM,T,ID>);
    }

    virtual QImage& thumbnail();

    virtual QList<QImage>& thumbnails() { return d->make_thumbnails(128,false); }

    //  Inherited slots (through virtual member functions).

    void* output() { return d->image.GetPointer(); }
    void* data()   { return d->image.GetPointer(); }

    void setData(void* data) {
        typedef typename PrivateMember::ImageType ImageType;
        typename ImageType::Pointer image = dynamic_cast<ImageType*>(static_cast<itk::Object*>(data));
        if (image.IsNull()) {
            qDebug() << "Cannot cast data to correct data type";
            return;
        }
        d->image = image;
        d->reset();
    }

    void update() { }

    void onMetaDataSet(const QString& key,const QString& value) {
        Q_UNUSED(key);
        Q_UNUSED(value);
    }

    void onPropertySet(const QString& key,const QString& value) {
        Q_UNUSED(key);
        Q_UNUSED(value);
    }

    // derived from medAbstractDataImage
    
    medAbstractDataImage::MatrixType orientationMatrix()
    {
        medAbstractDataImage::MatrixType orientationMatrix;
        
        if (!d->image)
            return orientationMatrix;
        
        for (unsigned int i = 0;i < DIM;++i)
        {
            std::vector <double> orientationVector(DIM,0);
            for(unsigned int j = 0;j < DIM;++j)
                orientationVector[j] = d->image->GetDirection()(i,j);
            
            orientationMatrix.push_back(orientationVector);
        }
        
        return orientationMatrix;
    }

    int xDimension() {
        if (d->image.IsNull())
            return -1;
        return d->image->GetLargestPossibleRegion().GetSize()[0];
    }

    int yDimension() {
        if (d->image.IsNull())
            return -1;
        return d->image->GetLargestPossibleRegion().GetSize()[1];
    }

    int zDimension() {
        if (d->image.IsNull())
            return -1;
        return d->image->GetLargestPossibleRegion().GetSize()[2];
    }

    int tDimension() {
        if (d->image.IsNull())
            return -1;
        if (DIM<4)
            return 1;
        else
            return d->image->GetLargestPossibleRegion().GetSize()[3];
    }

    int minRangeValue() { return d->minRangeValue(); }
    int maxRangeValue() { return d->maxRangeValue(); }

    int scalarValueCount(int value) { return d->scalarValueCount(value); }
    int scalarValueMinCount()       { return d->scalarValueMinCount();   }
    int scalarValueMaxCount()       { return d->scalarValueMaxCount();   }

private:

    PrivateMember* d;
};

template <unsigned DIM,typename T,const char* ID>
medAbstractData* createItkDataImage() {
    return new itkDataImage<DIM,T,ID>;
}

template <unsigned DIM,typename T,const char* ID>
QImage& itkDataImage<DIM,T,ID>::thumbnail() {
    if (d->image.IsNull())
        return medAbstractDataImage::thumbnail();

    const int xydim = 128;
    if (d->thumbnails.isEmpty()          ||
        d->thumbnails[0].height()!=xydim ||
        d->thumbnails[0].width() !=xydim )
        d->make_thumbnails(xydim,true);

    int index = 0;
    if (DIM>2) {
        typename PrivateMember::ImageType::SizeType size = d->image->GetLargestPossibleRegion().GetSize();
        index = size[2]/2;
    }

    qDebug() << "thumbnail " << index << " / " << d->thumbnails.length();
    if (index < d->thumbnails.length())
        return d->thumbnails[index];
    else if (d->thumbnails.size()>0)
        return d->thumbnails[0];
    else
        return medAbstractDataImage::thumbnail();
}


