/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkMultiplyImageProcess.h>

#include <dtkLog>

#include <itkMultiplyImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkCommand.h>

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
    return "Multiply images (ITK)";
}

QString medItkMultiplyImageProcess::description() const
{
    return "Use ITK MultiplyImageFilter to perform the multiplication of two images.";
}

medItkMultiplyImageProcess::ImageType::Pointer medItkMultiplyImageProcess::CastImage(medAbstractImageData *data)
{
    QString id = data->identifier();
    ImageType::Pointer output;

    if ( id == "itkDataImageChar3" )
    {
        output = this->_cast<char>(data);
    }
    else if ( id == "itkDataImageUChar3" )
    {
        output = this->_cast<unsigned char>(data);
    }
    else if ( id == "itkDataImageShort3" )
    {
        output = this->_cast<short>(data);
    }
    else if ( id == "itkDataImageUShort3" )
    {
        output = this->_cast<unsigned short>(data);
    }
    else if ( id == "itkDataImageInt3" )
    {
        output = this->_cast<int>(data);
    }
    else if ( id == "itkDataImageUInt3" )
    {
        output = this->_cast<unsigned int>(data);
    }
    else if ( id == "itkDataImageLong3" )
    {
        output = this->_cast<long>(data);
    }
    else if ( id== "itkDataImageULong3" )
    {
        output = this->_cast<unsigned long>(data);
    }
    else if ( id == "itkDataImageFloat3" )
    {
        output = this->_cast<float>(data);
    }
    else if ( id == "itkDataImageDouble3" )
    {
        output = this->_cast<double>(data);
    }

    return output;
}

template <class inputType>
medItkMultiplyImageProcess::ImageType::Pointer
medItkMultiplyImageProcess::_cast(medAbstractImageData *data)
{
    typedef itk::Image <inputType, 3> InputImageType;
    typedef itk::CastImageFilter <InputImageType, ImageType> CastFilterType;

    typename CastFilterType::Pointer castFilter = CastFilterType::New();
    castFilter->SetInput(dynamic_cast<InputImageType *>((itk::Object*)(data->data())));
    castFilter->Update();
    ImageType::Pointer output = castFilter->GetOutput();

    return output;
}

medAbstractJob::medJobExitStatus medItkMultiplyImageProcess::run()
{
   if (!this->input1() || !this->input2())
        return medAbstractJob::MED_JOB_EXIT_FAILURE;

    ImageType::Pointer leftSideInput = this->CastImage(this->input1());
    ImageType::Pointer rightSideInput = this->CastImage(this->input2());

    typedef itk::MultiplyImageFilter<ImageType, ImageType, ImageType> FilterType;
    typename FilterType::Pointer filter = FilterType::New();
    m_filter = filter;

    filter->SetInput1(leftSideInput);
    filter->SetInput2(rightSideInput);

    itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
    callback->SetClientData((void*)this);
    callback->SetCallback(medItkMultiplyImageProcess::eventCallback);
    filter->AddObserver(itk::ProgressEvent(), callback);

    try
    {
        filter->Update();
    }
    catch(itk::ProcessAborted &e)
    {
        return medAbstractJob::MED_JOB_EXIT_CANCELLED;
    }

    medAbstractImageData *out= qobject_cast<medAbstractImageData *>(medAbstractDataFactory::instance()->create("itkDataImageDouble3"));
    out->setData(filter->GetOutput());
    this->setOutput(out);
    return medAbstractJob::MED_JOB_EXIT_SUCCESS;
}

void medItkMultiplyImageProcess::cancel()
{
    if(this->isRunning() && m_filter.IsNotNull())
    {
       m_filter->AbortGenerateDataOn();
    }
}
