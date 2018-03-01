/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDataTensorImageFloat3.h>

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

class itkDataTensorImageFloat3Private
{
public:
    typedef itk::Tensor<float, 3>    TensorType;
    typedef itk::Image<TensorType, 3> TensorImageType;

    TensorImageType::Pointer tensors;

};

// /////////////////////////////////////////////////////////////////
// itkDataTensorImage
// /////////////////////////////////////////////////////////////////

itkDataTensorImageFloat3::itkDataTensorImageFloat3() : medAbstractTypedDiffusionModelImageData<3,float>(), d(new itkDataTensorImageFloat3Private)
{
    d->tensors = 0;
}

itkDataTensorImageFloat3::~itkDataTensorImageFloat3()
{
    delete d;
    d = 0;
}

bool itkDataTensorImageFloat3::registered()
{
    return medAbstractDataFactory::instance()->registerDataType<itkDataTensorImageFloat3>();
}

void *itkDataTensorImageFloat3::data()
{
    return d->tensors.GetPointer();
}

void *itkDataTensorImageFloat3::output()
{
    return d->tensors.GetPointer();
}

void itkDataTensorImageFloat3::setData(void *data)
{
    typedef itkDataTensorImageFloat3Private::TensorImageType TensorImageType;

    if( TensorImageType* tensors = dynamic_cast<TensorImageType*>( (itk::Object*)(data) ) ){
        d->tensors = tensors;
    }
    else
      dtkDebug() << "Cannot cast pointer to correct tensor type";
}

int itkDataTensorImageFloat3::xDimension()
{
    if (!d->tensors.IsNull())
      return d->tensors->GetLargestPossibleRegion().GetSize()[0];
    return -1;
}

int itkDataTensorImageFloat3::yDimension()
{
    if (!d->tensors.IsNull())
        return d->tensors->GetLargestPossibleRegion().GetSize()[1];
    return -1;
}

int itkDataTensorImageFloat3::zDimension()
{
    if (!d->tensors.IsNull())
        return d->tensors->GetLargestPossibleRegion().GetSize()[2];
    return -1;
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

medAbstractData *createItkDataTensorImageFloat3()
{
    return new itkDataTensorImageFloat3;
}

