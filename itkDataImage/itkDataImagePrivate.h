#ifndef _itkDataImagePrivate_h
#define _itkDataImagePrivate_h

#include "itkDataImageBaseImpl.h"

#include <itkExtractImageFilter.h>
#include <itkGreyColormapFunctor.h>
#include <itkImage.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkRGBAPixel.h>
#include <itkRGBPixel.h>
#include <itkRecursiveGaussianImageFilter.h>
#include <itkRegionOfInterestImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkScalarImageToHistogramGenerator.h>
#include <itkScalarToRGBColormapImageFilter.h>
#include <itkShrinkImageFilter.h>
#include <itkVector.h>

#include <dtkCore/dtkAbstractData.h>

/**
 * Template implementation of itkDataImageBaseImpl for non-vector pixel types.
 * */
template < typename TPixel, unsigned int VDimension>
class itkDataImagePrivate : public itkDataImageBaseImpl
{
public:
    typedef TPixel                              ScalarType;
    typedef typename itk::Image<ScalarType, VDimension> ImageType;
    typedef typename itk::Statistics::ScalarImageToHistogramGenerator< ImageType >
        HistogramGeneratorType;
    typedef typename HistogramGeneratorType::HistogramType HistogramType;

public:
    itkDataImagePrivate( itkDataImageBase * self );
    ~itkDataImagePrivate();

public:
    virtual QImage        &thumbnail(void);
    virtual QList<QImage> &thumbnails (void);
    virtual void *output(void);
    virtual void *data(void);
    virtual void setData(void* data);
    virtual void update(void);
    virtual int xDimension(void);
    virtual int yDimension(void);
    virtual int zDimension(void);
    virtual int tDimension(void);

    virtual int minRangeValue(void);
    virtual int maxRangeValue(void);

    virtual int scalarValueCount(int value);
    virtual int scalarValueMinCount(void);
    virtual int scalarValueMaxCount(void);

protected:
    void computeRange();
    void computeValueCounts();
    static void generateThumbnails (typename itk::Image<TPixel, VDimension>::Pointer image,
        int xydim, bool singlez, QList<QImage> & thumbnails);

protected:
    itkDataImageBase * self;
    typename ImageType::Pointer          m_image;
    bool m_range_computed;
    ScalarType m_range_min;
    ScalarType m_range_max;
    typename HistogramType::Pointer m_histogram;
    int                    m_histogram_min;
    int                    m_histogram_max;
    QList<QImage>          m_thumbnails;
    QImage                 m_defaultThumbnail;
};

#include "itkDataImagePrivate.txx"

#endif
