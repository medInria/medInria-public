/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkSubstractImageProcess.h>

#include <medAbstractImageData.h>
#include <medAbstractDataFactory.h>

#include <itkImage.h>
#include <itkSubtractImageFilter.h>

class medItkSubstractImageProcessPrivate
{
public:

};

medItkSubstractImageProcess::medItkSubstractImageProcess(QObject *parent): medAbstractSubstractImageProcess(parent),
    d(new medItkSubstractImageProcessPrivate)
{

}

medItkSubstractImageProcess::~medItkSubstractImageProcess()
{

}

void medItkSubstractImageProcess::start()
{
    if(this->input1() && this->input2())
    {
        QString id =  this->input1()->identifier();

        if ( id == "itkDataImageChar3" )
        {
            this->_start<char>();
        }
        else if ( id == "itkDataImageUChar3" )
        {
            this->_start<unsigned char>();
        }
        else if ( id == "itkDataImageShort3" )
        {
            this->_start<short>();
        }
        else if ( id == "itkDataImageUShort3" )
        {
            this->_start<unsigned short>();
        }
        else if ( id == "itkDataImageInt3" )
        {
            this->_start<int>();
        }
        else if ( id == "itkDataImageUInt3" )
        {
            this->_start<unsigned int>();
        }
        else if ( id == "itkDataImageLong3" )
        {
            this->_start<long>();
        }
        else if ( id== "itkDataImageULong3" )
        {
            this->_start<unsigned long>();
        }
        else if ( id == "itkDataImageFloat3" )
        {
            this->_start<float>();
        }
        else if ( id == "itkDataImageDouble3" )
        {
            this->_start<double>();
        }
        else
        {
            emit failure();
        }
    }
}

template <class inputType>
void medItkSubstractImageProcess::_start()
{
    typedef itk::Image<inputType, 3> ImageType;

    typename ImageType::Pointer in1 = dynamic_cast<ImageType *>((itk::Object*)(this->input1()->data()));
    typename ImageType::Pointer in2 = dynamic_cast<ImageType *>((itk::Object*)(this->input2()->data()));

    if(in1.IsNull() || in2.IsNull())
    {
        emit failure();
        return;
    }

    typedef itk::SubtractImageFilter<ImageType> FilterType;
    typename FilterType::Pointer filter = FilterType::New();

    filter->SetInput1(in1);
    filter->SetInput2(in2);
    filter->Update();

    medAbstractImageData *out= dynamic_cast<medAbstractImageData *>(medAbstractDataFactory::instance()->create(this->input1()->identifier()));
    out->setData(filter->GetOutput());
    this->setOutput(out);
    emit success();
}
