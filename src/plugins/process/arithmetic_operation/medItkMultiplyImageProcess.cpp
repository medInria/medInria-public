/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkMultiplyImageProcess.h>

#include <dtkLog>

#include <itkImage.h>
#include <itkMultiplyImageFilter.h>

#include <medAbstractImageData.h>
#include <medAbstractDataFactory.h>
#include <medCore.h>


medItkMultiplyImageProcess::medItkMultiplyImageProcess(QObject *parent)
    : medAbstractMultiplyImageProcess(parent)

{
    m_filter = NULL;
}

medItkMultiplyImageProcess::~medItkMultiplyImageProcess()
{

}

QString medItkMultiplyImageProcess::caption() const
{
    return "Itk add images process";
}

QString medItkMultiplyImageProcess::description() const
{
    return "Use itk::MultiplyImageFilter to perform the addition of tow images.";
}

void medItkMultiplyImageProcess::run()
{
    if(this->input1() && this->input2())
    {
        QString id =  this->input1()->identifier();

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
void medItkMultiplyImageProcess::_run()
{
    typedef itk::Image<inputType, 3> ImageType;

    typename ImageType::Pointer in1 = dynamic_cast<ImageType *>((itk::Object*)(this->input1()->data()));
    typename ImageType::Pointer in2 = dynamic_cast<ImageType *>((itk::Object*)(this->input2()->data()));

    if(in1.IsNotNull() && in2.IsNotNull())
    {
        typedef itk::MultiplyImageFilter<ImageType, ImageType, ImageType> FilterType;
        typename FilterType::Pointer filter = FilterType::New();

        filter->SetInput1(in1);
        filter->SetInput2(in2);
        filter->Update();

        medAbstractImageData *out= qobject_cast<medAbstractImageData *>(medAbstractDataFactory::instance()->create("itkDataImageFloat3"));
        out->setData(filter->GetOutput());
        this->setOutput(out);
        emit success();
    }
    else
    {
        emit failure();
    }
}

void medItkMultiplyImageProcess::cancel()
{
    if(this->isRunning() && m_filter.IsNotNull())
    {
       m_filter->AbortGenerateDataOn();
       emit failure();
    }
}
