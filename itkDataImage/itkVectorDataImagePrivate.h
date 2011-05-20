#ifndef _itkVectorDataImagePrivate_h_
#define _itkVectorDataImagePrivate_h_

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
 * Template implementation of itkDataImageBaseImpl for vector pixel types.
 * */
template < typename TPixel , unsigned int VDimension >
class itkVectorDataImagePrivate : public itkDataImageBaseImpl
{
public:
    typedef TPixel                                                          ScalarType;
    typedef itk::Image<ScalarType, VDimension>                             ImageType;
public:
    itkVectorDataImagePrivate(itkDataImageBase * _self);
    ~itkVectorDataImagePrivate();

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
    itkDataImageBase * self;
    typename ImageType::Pointer          m_image;
    QList<QImage>               m_thumbnails;
};

#include "itkVectorDataImagePrivate.txx"

#endif
