/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTtkFloat3TensorImageData.h>

#include <medAbstractDataFactory.h>

#include <itkTensor.h>
#include <itkImage.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkImageFileWriter.h>
#include <itkVectorImage.h>
#include <itkVector.h>


// /////////////////////////////////////////////////////////////////
// itkDataTensorImagePrivate
// /////////////////////////////////////////////////////////////////

class medTtkFloat3TensorImageDataPrivate
{
public:
    typedef itk::Tensor<float, 3>    TensorType;
    typedef itk::Image<TensorType, 3> TensorImageType;

    TensorImageType::Pointer tensors;

};

// /////////////////////////////////////////////////////////////////
// itkDataTensorImage
// /////////////////////////////////////////////////////////////////

medTtkFloat3TensorImageData::medTtkFloat3TensorImageData() : medAbstractTypedDiffusionModelImageData<3,float>(), d(new medTtkFloat3TensorImageDataPrivate)
{
    d->tensors = 0;
}

medTtkFloat3TensorImageData::~medTtkFloat3TensorImageData()
{
    delete d;
    d = 0;
}

bool medTtkFloat3TensorImageData::registered()
{
    return medAbstractDataFactory::instance()->registerDataType<medTtkFloat3TensorImageData>();
}

void *medTtkFloat3TensorImageData::data()
{
    return d->tensors.GetPointer();
}

void *medTtkFloat3TensorImageData::output()
{
    return d->tensors.GetPointer();
}

void medTtkFloat3TensorImageData::setData(void *data)
{
    typedef medTtkFloat3TensorImageDataPrivate::TensorImageType TensorImageType;

    if( TensorImageType* tensors = dynamic_cast<TensorImageType*>( (itk::Object*)(data) ) ){
        d->tensors = tensors;
    }
    else
      qDebug() << "Cannot cast pointer to correct tensor type";
}

int medTtkFloat3TensorImageData::xDimension()
{
    if (!d->tensors.IsNull())
      return d->tensors->GetLargestPossibleRegion().GetSize()[0];
    return -1;
}

int medTtkFloat3TensorImageData::yDimension()
{
    if (!d->tensors.IsNull())
        return d->tensors->GetLargestPossibleRegion().GetSize()[1];
    return -1;
}

int medTtkFloat3TensorImageData::zDimension()
{
    if (!d->tensors.IsNull())
        return d->tensors->GetLargestPossibleRegion().GetSize()[2];
    return -1;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

medAbstractData *createmedTtkFloat3TensorImageData()
{
    return new medTtkFloat3TensorImageData;
}

