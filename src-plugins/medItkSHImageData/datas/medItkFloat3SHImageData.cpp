/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkFloat3SHImageData.h>

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

class medItkFloat3SHImageDataPrivate
{
public:
    typedef itk::VectorImage<float, 3> SHImageType;
    typedef SHImageType::PixelType    SHType;

    SHImageType::Pointer shs;
};

// /////////////////////////////////////////////////////////////////
// itkDataSHImage
// /////////////////////////////////////////////////////////////////

medItkFloat3SHImageData::medItkFloat3SHImageData(): medAbstractTypedDiffusionModelImageData<3,float>(), d(new medItkFloat3SHImageDataPrivate)
{
    d->shs = 0;
}

medItkFloat3SHImageData::~medItkFloat3SHImageData()
{
    delete d;
    d = 0;
}

bool medItkFloat3SHImageData::registered()
{
    return medAbstractDataFactory::instance()->registerDataType<medItkFloat3SHImageData>();
}

void *medItkFloat3SHImageData::data()
{
    return d->shs.GetPointer();
}

void *medItkFloat3SHImageData::output()
{
    return d->shs.GetPointer();
}

void medItkFloat3SHImageData::setData(void *data)
{
    typedef medItkFloat3SHImageDataPrivate::SHImageType SHImageType;

    if( SHImageType* shs = dynamic_cast<SHImageType*>( (itk::Object*)(data) ) ){
        d->shs = shs;
    }
    else
      qDebug() << "Cannot cast pointer to correct SH float type";
}

int medItkFloat3SHImageData::xDimension()
{
    if (!d->shs.IsNull())
      return d->shs->GetLargestPossibleRegion().GetSize()[0];
    return -1;
}

int medItkFloat3SHImageData::yDimension()
{
    if (!d->shs.IsNull())
        return d->shs->GetLargestPossibleRegion().GetSize()[1];
    return -1;
}

int medItkFloat3SHImageData::zDimension()
{
    if (!d->shs.IsNull())
        return d->shs->GetLargestPossibleRegion().GetSize()[2];
    return -1;
}
