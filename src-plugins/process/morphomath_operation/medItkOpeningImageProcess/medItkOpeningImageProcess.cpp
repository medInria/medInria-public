/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkOpeningImageProcess.h>

#include <dtkLog>

#include <itkImage.h>
#include <itkGrayscaleMorphologicalOpeningImageFilter.h>
#include <itkBinaryBallStructuringElement.h>

#include <medAbstractImageData.h>
#include <medAbstractDataFactory.h>
#include <medProcessLayer.h>
#include <medDoubleParameter.h>

class medItkOpeningImageProcessPrivate
{
public:
    medProcessDetails details;
};

medItkOpeningImageProcess::medItkOpeningImageProcess(QObject *parent): medAbstractOpeningImageProcess(parent),
    d(new medItkOpeningImageProcessPrivate)
{
    d->details = medProcessLayer::readDetailsFromJson(":/medItkOpeningImageProcessPlugin.json");
}

medItkOpeningImageProcess::~medItkOpeningImageProcess()
{

}

medProcessDetails medItkOpeningImageProcess::details() const
{
    return d->details;
}

void medItkOpeningImageProcess::run()
{
    if(this->input())
    {
        QString id =  this->input()->identifier();

        if ( id == "itkDataImageChar3" )
        {
            this->_run<char>();
        }
        else if ( id == "itkDataImageUChar3" )
        {
            this->_run<unsigned char>();
        }
        else if ( id == "itkDataImageShort3" )
        {
            this->_run<short>();
        }
        else if ( id == "itkDataImageUShort3" )
        {
            this->_run<unsigned short>();
        }
        else if ( id == "itkDataImageInt3" )
        {
            this->_run<int>();
        }
        else if ( id == "itkDataImageUInt3" )
        {
            this->_run<unsigned int>();
        }
        else if ( id == "itkDataImageLong3" )
        {
            this->_run<long>();
        }
        else if ( id== "itkDataImageULong3" )
        {
            this->_run<unsigned long>();
        }
        else if ( id == "itkDataImageFloat3" )
        {
            this->_run<float>();
        }
        else if ( id == "itkDataImageDouble3" )
        {
            this->_run<double>();
        }
        else
        {
            emit failure();
        }
    }
}

template <class inputType>
void medItkOpeningImageProcess::_run()
{
    typedef itk::Image<inputType, 3> ImageType;

    typename ImageType::Pointer in = dynamic_cast<ImageType *>((itk::Object*)(this->input()->data()));

    if(in.IsNull())
    {
        emit failure();
        return;
    }

    typedef itk::BinaryBallStructuringElement<inputType, 3> KernelType;
    typedef itk::GrayscaleMorphologicalOpeningImageFilter<ImageType, ImageType, KernelType> FilterType;
    typename FilterType::Pointer filter = FilterType::New();

    KernelType kernel;
    kernel.SetRadius(this->kernelRadius()->value());
    kernel.CreateStructuringElement();

    filter->SetKernel(kernel);
    filter->SetInput(in);
    filter->Update();

    medAbstractImageData *out= dynamic_cast<medAbstractImageData *>(medAbstractDataFactory::instance()->create(this->input()->identifier()));
    out->setData(filter->GetOutput());
    this->setOutput(out);
    emit success();
}

void medItkOpeningImageProcess::cancel()
{
    dtkTrace() << "No way to cancell " << d->details.name;
}
