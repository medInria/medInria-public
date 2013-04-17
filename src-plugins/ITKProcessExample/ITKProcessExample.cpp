/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "ITKProcessExample.h"

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include "itkImage.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"

// /////////////////////////////////////////////////////////////////
// ITKProcessExamplePrivate
// /////////////////////////////////////////////////////////////////

class ITKProcessExamplePrivate
{
public:
    dtkAbstractData *input;
    dtkAbstractData *output;
    double sigma;
    template <class PixelType> int update();
};

template <class PixelType> int ITKProcessExamplePrivate::update()
{
    qDebug() << "entering method d->update<" << typeid(PixelType).name() << ">";

    typedef itk::Image< PixelType, 3 > ImageType;

//     typedef itk::DiscreteGaussianImageFilter< ImageType, ImageType >  FilterType;
    
    typedef itk::SmoothingRecursiveGaussianImageFilter< ImageType, ImageType >  FilterType;

    typename FilterType::Pointer gaussianFilter = FilterType::New();

    gaussianFilter->SetInput(dynamic_cast<ImageType *>((itk::Object*)(input->data())));

//     gaussianFilter->SetVariance(variance);

    gaussianFilter->SetSigma(sigma);

    gaussianFilter->Update();

    output->setData(gaussianFilter->GetOutput());

    return EXIT_SUCCESS;
}

// /////////////////////////////////////////////////////////////////
// ITKProcessExample
// /////////////////////////////////////////////////////////////////

ITKProcessExample::ITKProcessExample() : dtkAbstractProcess(), d(new ITKProcessExamplePrivate)
{
    d->output = NULL;
    d->sigma = 4.0;
}

ITKProcessExample::~ITKProcessExample()
{
    delete d;
    d = NULL;
}

bool ITKProcessExample::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("ITKProcessExampleGaussianBlur", createITKProcessExample);
}

QString ITKProcessExample::description() const
{
    return "ITKProcessExampleGaussianBlur";
}

void ITKProcessExample::setInput(dtkAbstractData *data)
{
    if (!data)
        return;

    const QString& identifier = data->identifier();

    d->output = dtkAbstractDataFactory::instance()->create (identifier);

    d->input = data;
    qDebug() << "in method setInput, d->input =" << d->input;
}

void ITKProcessExample::setParameter(double  data, int channel)
{
    switch (channel){

        case (0):
                d->sigma = data;
                break;
        default :
                return;
        }
}

int ITKProcessExample::update()
{
    qDebug() << "entering method update";

    if (!d->input)
	{
	    qDebug() << "in update method : d->input == NULL";
            return -1;
	}

    const QString& id = d->input->identifier();

    if (id == "itkDataImageChar3") {
        d->update<char>();
     }
    else if (id == "itkDataImageUChar3") {
        d->update<unsigned char>();
     }
    else if (id == "itkDataImageShort3") {
        d->update<short>();
     }
    else if (id == "itkDataImageUShort3") {
        d->update<unsigned short>();
     }
    else if (id == "itkDataImageInt3") {
        d->update<int>();
     }
    else if (id == "itkDataImageUInt3") {
        d->update<unsigned int>();
     }
    else if (id == "itkDataImageLong3") {
        d->update<long>();
     }
    else if (id== "itkDataImageULong3") {
        d->update<unsigned long>();
     }
    else if (id == "itkDataImageFloat3") {
        d->update<float>();
     }
    else if (id == "itkDataImageDouble3") {
        d->update<double>();
     }
    else
    {
        qDebug() << "Error : pixel type not yet implemented ("
                 << id
                 << ")";
        return -1;
    }

    return EXIT_SUCCESS;
}

dtkAbstractData * ITKProcessExample::output()
{
    return (d->output);
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createITKProcessExample()
{
    return new ITKProcessExample;
}
