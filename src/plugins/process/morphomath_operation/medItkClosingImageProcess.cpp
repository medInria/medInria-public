/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkClosingImageProcess.h>

#include <dtkLog>

#include <itkImage.h>
#include <itkGrayscaleMorphologicalClosingImageFilter.h>
#include <itkBinaryBallStructuringElement.h>

#include <medAbstractImageData.h>
#include <medAbstractDataFactory.h>
#include <medDoubleParameter.h>


medItkClosingImageProcess::medItkClosingImageProcess(QObject *parent)
    : medAbstractClosingImageProcess(parent)

{
    m_filter = NULL;
}

medItkClosingImageProcess::~medItkClosingImageProcess()
{

}


QString medItkClosingImageProcess::caption() const
{
    return "Itk closing image process";
}

QString medItkClosingImageProcess::description() const
{
    return "Use itk::GrayscaleMorphologicalClosingImageFilter to perform a closing of an image.";
}
void medItkClosingImageProcess::run()
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
void medItkClosingImageProcess::_run()
{
    typedef itk::Image<inputType, 3> ImageType;

    typename ImageType::Pointer in = dynamic_cast<ImageType *>((itk::Object*)(this->input()->data()));

    if(in.IsNotNull())
    {
        typedef itk::BinaryBallStructuringElement<inputType, 3> KernelType;
        typedef itk::GrayscaleMorphologicalClosingImageFilter<ImageType, ImageType, KernelType> FilterType;
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
    else
    {
        emit failure();
    }
}

void medItkClosingImageProcess::cancel()
{
    if(this->isRunning() && m_filter.IsNotNull())
    {
        m_filter->AbortGenerateDataOn();
        emit failure();
    }
}
