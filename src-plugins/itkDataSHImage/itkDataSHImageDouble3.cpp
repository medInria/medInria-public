/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDataSHImageDouble3.h>
#include <medAbstractDataFactory.h>

#include <itkImage.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkImageFileWriter.h>
#include <itkVectorImage.h>
#include <itkVector.h>

// /////////////////////////////////////////////////////////////////
// itkDataSHImagePrivate
// /////////////////////////////////////////////////////////////////

class itkDataSHImageDouble3Private
{
public:
    typedef itk::VectorImage<double, 3> SHImageType;
    typedef SHImageType::PixelType    SHType;

    SHImageType::Pointer shs;

    QImage        thumbnail;
    QList<QImage> thumbnails;
};

// /////////////////////////////////////////////////////////////////
// itkDataSHImage
// /////////////////////////////////////////////////////////////////

itkDataSHImageDouble3::itkDataSHImageDouble3(): medAbstractTypedImageData<3,double>(), d(new itkDataSHImageDouble3Private)
{
    d->shs = 0;
    d->thumbnail = QImage(":/itkDataSHImage/icons/shs.png");
    d->thumbnails << d->thumbnail;
}

itkDataSHImageDouble3::~itkDataSHImageDouble3()
{
    delete d;
    d = 0;
}

bool itkDataSHImageDouble3::registered()
{
    return medAbstractDataFactory::instance()->registerDataType("itkDataSHImageDouble3", createItkDataSHImageDouble3);
}

QString itkDataSHImageDouble3::description() const
{
    return tr("itk spherical harmonic 3d image data (double)");
}
QString itkDataSHImageDouble3::identifier() const
{
    return "itkDataSHImageDouble3";
}

void *itkDataSHImageDouble3::data()
{
    return d->shs.GetPointer();
}

void *itkDataSHImageDouble3::output()
{
    return d->shs.GetPointer();
}

void itkDataSHImageDouble3::setData(void *data)
{
    typedef itkDataSHImageDouble3Private::SHImageType SHImageType;

    if( SHImageType* shs = dynamic_cast<SHImageType*>( (itk::Object*)(data) ) ){
        d->shs = shs;
    }
    else
      qDebug() << "Cannot cast pointer to correct SH double type";
}

int itkDataSHImageDouble3::xDimension()
{
    if (!d->shs.IsNull())
      return d->shs->GetLargestPossibleRegion().GetSize()[0];
    return -1;
}

int itkDataSHImageDouble3::yDimension()
{
    if (!d->shs.IsNull())
        return d->shs->GetLargestPossibleRegion().GetSize()[1];
    return -1;
}

int itkDataSHImageDouble3::zDimension()
{
    if (!d->shs.IsNull())
        return d->shs->GetLargestPossibleRegion().GetSize()[2];
    return -1;
}

QImage& itkDataSHImageDouble3::thumbnail()
{
    // TODO: TEMPORARY black image just to allow drag and drop
    return d->thumbnail;
}

QList<QImage>& itkDataSHImageDouble3::thumbnails()
{
    return d->thumbnails;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

medAbstractData *createItkDataSHImageDouble3()
{
    return new itkDataSHImageDouble3;
}
