/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTtkDouble3TensorImageData.h>

#include <medAbstractDataFactory.h>

#include <itkImage.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkImageFileWriter.h>
#include <itkVectorImage.h>
#include <itkVector.h>
#include <itkTensor.h>


// /////////////////////////////////////////////////////////////////
// itkDataTensorImagePrivate
// /////////////////////////////////////////////////////////////////

class medTtkDouble3TensorImageDataPrivate
{
public:
    typedef itk::Tensor<double, 3>    TensorType;
    typedef itk::Image<TensorType, 3> TensorImageType;

    TensorImageType::Pointer tensors;
};

// /////////////////////////////////////////////////////////////////
// itkDataTensorImage
// /////////////////////////////////////////////////////////////////

medTtkDouble3TensorImageData::medTtkDouble3TensorImageData() : medAbstractTypedDiffusionModelImageData<3,double>(), d(new medTtkDouble3TensorImageDataPrivate)
{
    d->tensors = 0;

}

medTtkDouble3TensorImageData::~medTtkDouble3TensorImageData()
{
    delete d;
    d = 0;
}

bool medTtkDouble3TensorImageData::registered()
{
    return medAbstractDataFactory::instance()->registerDataType<medTtkDouble3TensorImageData>();
}

void *medTtkDouble3TensorImageData::data()
{
    return d->tensors.GetPointer();
}

void *medTtkDouble3TensorImageData::output()
{
    return d->tensors.GetPointer();
}

void medTtkDouble3TensorImageData::setData(void *data)
{
    typedef medTtkDouble3TensorImageDataPrivate::TensorImageType TensorImageType;

    if( TensorImageType* tensors = dynamic_cast<TensorImageType*>( (itk::Object*)(data) ) ){
        d->tensors = tensors;
    }
    else
      qDebug() << "Cannot cast pointer to correct tensor type";
}

int medTtkDouble3TensorImageData::xDimension()
{
    if (!d->tensors.IsNull())
      return d->tensors->GetLargestPossibleRegion().GetSize()[0];
    return -1;
}

int medTtkDouble3TensorImageData::yDimension()
{
    if (!d->tensors.IsNull())
        return d->tensors->GetLargestPossibleRegion().GetSize()[1];
    return -1;
}

int medTtkDouble3TensorImageData::zDimension()
{
    if (!d->tensors.IsNull())
        return d->tensors->GetLargestPossibleRegion().GetSize()[2];
    return -1;
}
