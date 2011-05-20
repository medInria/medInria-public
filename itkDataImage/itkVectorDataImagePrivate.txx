#ifndef _itkVectorDataImagePrivate_txx_
#define _itkVectorDataImagePrivate_txx_

#include "itkVectorDataImagePrivate.h"


template < typename TPixel , unsigned int VDimension >
itkVectorDataImagePrivate< TPixel , VDimension >::itkVectorDataImagePrivate( itkDataImageBase * _self )
    : self(_self),
    m_image( NULL )
{
}

template < typename TPixel , unsigned int VDimension >
itkVectorDataImagePrivate< TPixel , VDimension >::~itkVectorDataImagePrivate()
{
}


template < typename TPixel , unsigned int VDimension >
void itkVectorDataImagePrivate< TPixel , VDimension >::setData(void *data)
{
    typename ImageType::Pointer image = dynamic_cast<ImageType*>( (itk::Object*) data );
    if (image.IsNull()) {
        qDebug() << "Cannot cast data to correct data TPixel";
        return;
    }
    m_image = image;
}

template < typename TPixel , unsigned int VDimension >
void *itkVectorDataImagePrivate< TPixel , VDimension >::output(void)
{
    if (m_image.IsNull())
        return NULL;
    return m_image.GetPointer();
}

template < typename TPixel , unsigned int VDimension >
void *itkVectorDataImagePrivate< TPixel , VDimension >::data(void)
{
    if (m_image.IsNull())
        return NULL;
    return m_image.GetPointer();
}

template < typename TPixel , unsigned int VDimension >
void itkVectorDataImagePrivate< TPixel , VDimension >::update(void)
{
}

template < typename TPixel , unsigned int VDimension >
int itkVectorDataImagePrivate< TPixel , VDimension >::xDimension(void)
{
    if (m_image.IsNull())
        return -1;
    return m_image->GetLargestPossibleRegion().GetSize()[0];
}

template < typename TPixel , unsigned int VDimension >
int itkVectorDataImagePrivate< TPixel , VDimension >::yDimension(void)
{
    if (m_image.IsNull())
        return -1;
    return m_image->GetLargestPossibleRegion().GetSize()[1];
}

template < typename TPixel , unsigned int VDimension >
int itkVectorDataImagePrivate< TPixel , VDimension >::zDimension(void)
{
    if (m_image.IsNull())
        return -1;
    return m_image->GetLargestPossibleRegion().GetSize()[2];
}

template < typename TPixel , unsigned int VDimension >
int itkVectorDataImagePrivate< TPixel , VDimension >::tDimension(void)
{
    if (m_image.IsNull())
        return -1;
    if (VDimension<4)
        return 1;
    else
        return m_image->GetLargestPossibleRegion().GetSize()[3];
}

template < typename TPixel , unsigned int VDimension >
int itkVectorDataImagePrivate< TPixel , VDimension >::minRangeValue(void)
{
    return -1;
}

template < typename TPixel , unsigned int VDimension >
int itkVectorDataImagePrivate< TPixel , VDimension >::maxRangeValue(void)
{
    return -1;
}

template < typename TPixel , unsigned int VDimension >
int itkVectorDataImagePrivate< TPixel , VDimension >::scalarValueCount(int value)
{
    return -1;
}

template < typename TPixel , unsigned int VDimension >
int itkVectorDataImagePrivate< TPixel , VDimension >::scalarValueMinCount(void)
{
    return -1;
}

template < typename TPixel , unsigned int VDimension >
int itkVectorDataImagePrivate< TPixel , VDimension >::scalarValueMaxCount(void)
{
    return -1;
}

template < typename TPixel , unsigned int VDimension >
QImage &itkVectorDataImagePrivate< TPixel , VDimension >::thumbnail (void)
{
    return self->dtkAbstractDataImage::thumbnail();
}

template < typename TPixel , unsigned int VDimension >
QList<QImage> &itkVectorDataImagePrivate< TPixel , VDimension >::thumbnails (void)
{
    typedef itk::Image<TPixel, 2>                      Image2DType;
    if (m_image.IsNull() )
        return m_thumbnails;
    if (ImageType::GetImageDimension()<2 )
        return m_thumbnails;
    typename ImageType::Pointer image = m_image;
    {
        typename ImageType::SizeType size = m_image->GetLargestPossibleRegion().GetSize();
        typename ImageType::SizeType newSize = size;
        newSize[0] = 128;
        newSize[1] = 128;
        unsigned int sfactor[ VDimension ];
        for (unsigned int i=0; i<VDimension; i++)
            sfactor[i] = size[i]/newSize[i];
        typedef typename itk::ShrinkImageFilter<ImageType, ImageType> FilterType;
        typename FilterType::Pointer filter = FilterType::New();
        filter->SetInput ( image );
        filter->SetShrinkFactors ( sfactor );
        try
        {
            filter->Update();
        }
        catch (itk::ExceptionObject &e)
        {
            qDebug() << e.GetDescription();
            return m_thumbnails;
        }
        image = filter->GetOutput();
    }
    typename ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize();
    typename itk::ImageRegionIterator<ImageType> it (image, image->GetLargestPossibleRegion());
    unsigned long nvoxels_per_slice = size[0]*size[1];
    unsigned long voxelCount = 0;
    QImage *qimage = new QImage (size[0], size[1], QImage::Format_ARGB32);
    uchar *qImageBuffer = qimage->bits();
    while(!it.IsAtEnd())
    {
        *qImageBuffer++ = static_cast<unsigned char>(it.Value()[0]);
        *qImageBuffer++ = static_cast<unsigned char>(it.Value()[1]);
        *qImageBuffer++ = static_cast<unsigned char>(it.Value()[2]);
        *qImageBuffer++ = 0xFF;
        ++it;
        ++voxelCount;
        if ( (voxelCount%nvoxels_per_slice)==0 ) {
            m_thumbnails.push_back (*qimage);
            qimage = new QImage (size[0], size[1], QImage::Format_ARGB32);
            qImageBuffer = qimage->bits();
        }
    }
    return m_thumbnails;
}

#endif // _itkVectorDataImagePrivate_txx_
