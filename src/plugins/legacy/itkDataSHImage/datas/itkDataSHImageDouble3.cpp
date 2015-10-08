/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
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
};

// /////////////////////////////////////////////////////////////////
// itkDataSHImage
// /////////////////////////////////////////////////////////////////

itkDataSHImageDouble3::itkDataSHImageDouble3(): medAbstractTypedDiffusionModelImageData<3,double>(), d(new itkDataSHImageDouble3Private)
{
    d->shs = 0;
}

itkDataSHImageDouble3::~itkDataSHImageDouble3()
{
    delete d;
    d = 0;
}

bool itkDataSHImageDouble3::registered()
{
    return medAbstractDataFactory::instance()->registerDataType<itkDataSHImageDouble3>();
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
