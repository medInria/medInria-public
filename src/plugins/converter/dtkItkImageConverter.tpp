// Version: $Id$
//
//

// Commentary:
//
//

// Change Log:
//
//

// Code:

#include <dtkImageData.h>
#include "itkImportImageFilter.h"

#include <dtkImage.h>
#include <dtkTemplatedImageData.h>
#include <dtkImageDefaultStorage.h>
#include <dtkPixelImpl.h>
#include <itkImage.h>
#include <dtkItkImageConverter.h>

// /////////////////////////////////////////////////////////////////
// dtkItkImageConverterHandler implementation
// /////////////////////////////////////////////////////////////////

template < typename Pixel, int dim> inline void dtkItkImageConverterHandler<Pixel, dim>::convertToNative(dtkImage *source, typename itk::Image<typename dtkItkPixelTypeTrait<Pixel>::itkPixelType, dim>::Pointer target)
{
    dtkDebug() << Q_FUNC_INFO << __LINE__;

    dtkImageData *data = source->data();
    dtkDebug() << Q_FUNC_INFO << __LINE__;

    typedef itk::Image<typename dtkItkPixelTypeTrait<Pixel>::itkPixelType, dim> ItkImageType;
    typedef itk::ImportImageFilter< typename dtkItkPixelTypeTrait<Pixel>::itkPixelType, dim > ImportFilterType;
    typename ImportFilterType::Pointer importFilter = ImportFilterType::New();

    dtkDebug() << Q_FUNC_INFO << __LINE__;

    typename ImportFilterType::IndexType index;
    typename ImportFilterType::SizeType size;
    const dtkArray<qlonglong>& extent = source->extent();
    for(int i = 0; i < dim; ++i) {
        index[i] = extent[2 * i];
        size[i] = (extent[2 * i + 1] - extent[2 * i] + 1);
    }
    typename ImportFilterType::RegionType region;
    region.SetIndex(index);
    region.SetSize(size);
    importFilter->SetRegion(region);

    double origin[dim];
    for(int i = 0; i < dim; ++i) {
        origin[i] = source->origin()[i];
    }
    importFilter->SetOrigin(origin);

    double spacing[dim];
    for(int i = 0; i < dim; ++i) {
        spacing[i] = source->spacing()[i];
    }
    importFilter->SetSpacing(spacing);

    typename ImportFilterType::DirectionType direction;
    for(int i = 0; i < dim; ++i) {
        for(int j = 0; j < dim; ++j) {
            direction(i,j) = source->transformMatrix()[i * dim + j];
        }
    }
    importFilter->SetDirection(direction);

    unsigned int numberOfValues = 1;
    for(int i = 0; i < dim; ++i) {
        numberOfValues *= size[i];
    }
    numberOfValues *= Pixel::PixelDim;

    dtkDebug() << source->rawData() << numberOfValues;

    importFilter->SetImportPointer((typename dtkItkPixelTypeTrait<Pixel>::itkPixelType *)(source->rawData()), numberOfValues, false );
    dtkDebug() << Q_FUNC_INFO << __LINE__;

    importFilter->Update();
    dtkDebug() << Q_FUNC_INFO << __LINE__;

    target->Graft(importFilter->GetOutput());
}

// /////////////////////////////////////////////////////////////////

template < typename Pixel, int dim> void dtkItkImageConverterHandler<Pixel, dim>::convertFromNative(typename itk::Image<typename dtkItkPixelTypeTrait<Pixel>::itkPixelType, dim>::Pointer source, dtkImage *target)
{
    typedef itk::Image<typename dtkItkPixelTypeTrait<Pixel>::itkPixelType, dim> ImageType;

    typedef typename ImageType::SizeType SizeType;
    typedef typename ImageType::IndexType IndexType;
    typedef typename ImageType::PointType PointType;
    typedef typename ImageType::SpacingType SpacingType;
    typedef typename ImageType::DirectionType DirectionType;

    typename ImageType::PixelContainer *container = source->GetPixelContainer();
    container->SetContainerManageMemory(false);

    dtkArray<qlonglong> extent(2 * dim);
    dtkArray<double> spacing(dim);
    dtkArray<double> origin(dim);
    dtkArray<double> direction(dim * dim);

    SizeType size = source->GetBufferedRegion().GetSize();
    IndexType index = source->GetBufferedRegion().GetIndex();
    for(int i = 0; i < dim; ++i) {
        extent[i * 2]     = qlonglong(index[i]);
        extent[i * 2 + 1] = qlonglong(index[i] + size[i]) - 1;
    }

    PointType itkOrigin = source->GetOrigin();
    for(int i = 0; i < itkOrigin.Size(); ++i) {
        origin[i] = itkOrigin[i];
    }

    SpacingType itkSpacing = source->GetSpacing();
    for(int i = 0; i < itkSpacing.Size(); ++i) {
        spacing[i] = itkSpacing[i];
    }

    DirectionType itkDirection = source->GetDirection();
    for(int i = 0; i < dim; ++i) {
        for(int j = 0; j < dim; ++j) {
            direction[i * dim + j] = itkDirection(i, j);
        }
    }

    typedef typename std::conditional<std::is_class<typename dtkItkPixelTypeTrait<Pixel>::itkPixelType>::value, dtkPixelClassDataExtractor<Pixel, dim>, dtkPixelRawDataExtractor<Pixel,dim> >::type DataExtractor;

    typename Pixel::PixelType *data = DataExtractor::getData(source);

    dtkImageDefaultStorage<Pixel, dim> *storage = new dtkImageDefaultStorage<Pixel, dim>(extent, data);

    target->setData(storage);
    target->setSpacing(spacing);
    target->setOrigin(origin);
    target->setTransformMatrix(direction);
}

// /////////////////////////////////////////////////////////////////
// dtkItkImageConverter implementation
// /////////////////////////////////////////////////////////////////

template < typename Pixel, int dim> inline void dtkItkImageConverter<Pixel,dim>::convertToNative(dtkImage *source, typename itk::Image<typename dtkItkPixelTypeTrait<Pixel>::itkPixelType, dim>::Pointer target)
{
    dtkItkImageConverterHandler<Pixel,dim>::convertToNative(source, target);
}

template < typename Pixel, int dim> void dtkItkImageConverter<Pixel,dim>::convertFromNative(typename itk::Image<typename dtkItkPixelTypeTrait<Pixel>::itkPixelType, dim>::Pointer source, dtkImage *target)
{
    dtkItkImageConverterHandler<Pixel,dim>::convertFromNative(source, target);
}

//
// dtkFilterITK_p.h ends here
