/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkDouble3SHImageData.h>
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

class medItkDouble3SHImageDataPrivate
{
public:
    typedef itk::VectorImage<double, 3> SHImageType;
    typedef SHImageType::PixelType    SHType;

    SHImageType::Pointer shs;
};

// /////////////////////////////////////////////////////////////////
// itkDataSHImage
// /////////////////////////////////////////////////////////////////

medItkDouble3SHImageData::medItkDouble3SHImageData(): medAbstractTypedDiffusionModelImageData<3,double>(), d(new medItkDouble3SHImageDataPrivate)
{
    d->shs = 0;
}

medItkDouble3SHImageData::~medItkDouble3SHImageData()
{
    delete d;
    d = 0;
}

bool medItkDouble3SHImageData::registered()
{
    return medAbstractDataFactory::instance()->registerDataType<medItkDouble3SHImageData>();
}

void *medItkDouble3SHImageData::data()
{
    return d->shs.GetPointer();
}

void *medItkDouble3SHImageData::output()
{
    return d->shs.GetPointer();
}

void medItkDouble3SHImageData::setData(void *data)
{
    typedef medItkDouble3SHImageDataPrivate::SHImageType SHImageType;

    if( SHImageType* shs = dynamic_cast<SHImageType*>( (itk::Object*)(data) ) ){
        d->shs = shs;
    }
    else
      qDebug() << "Cannot cast pointer to correct SH double type";
}

int medItkDouble3SHImageData::xDimension()
{
    if (!d->shs.IsNull())
      return d->shs->GetLargestPossibleRegion().GetSize()[0];
    return -1;
}

int medItkDouble3SHImageData::yDimension()
{
    if (!d->shs.IsNull())
        return d->shs->GetLargestPossibleRegion().GetSize()[1];
    return -1;
}

int medItkDouble3SHImageData::zDimension()
{
    if (!d->shs.IsNull())
        return d->shs->GetLargestPossibleRegion().GetSize()[2];
    return -1;
}
