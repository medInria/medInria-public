/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkSubtractImageProcess.h>

#include <dtkLog>

#include <itkSubtractImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkCommand.h>

#include <medAbstractImageData.h>
#include <medAbstractDataFactory.h>
#include <medCore.h>


medItkSubtractImageProcess::medItkSubtractImageProcess(QObject *parent)
    : medAbstractSubtractImageProcess(parent)

{
    m_filter = nullptr;
}

medItkSubtractImageProcess::~medItkSubtractImageProcess()
{

}

QString medItkSubtractImageProcess::caption() const
{
    return "Subtract images (ITK)";
}

QString medItkSubtractImageProcess::description() const
{
    return "Use ITK SubtractImageFilter to perform the subtraction of two images.";
}

medItkSubtractImageProcess::ImageType::Pointer medItkSubtractImageProcess::CastImage(medAbstractImageData *data)
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
medItkSubtractImageProcess::ImageType::Pointer
medItkSubtractImageProcess::_cast(medAbstractImageData *data)
{
    typedef itk::Image <inputType, 3> InputImageType;
    typedef itk::CastImageFilter <InputImageType, ImageType> CastFilterType;

    typename CastFilterType::Pointer castFilter = CastFilterType::New();
    castFilter->SetInput(dynamic_cast<InputImageType *>((itk::Object*)(data->data())));
    castFilter->Update();
    ImageType::Pointer output = castFilter->GetOutput();

    return output;
}

medAbstractJob::medJobExitStatus medItkSubtractImageProcess::run()
{
   if (!this->input1() || !this->input2())
        return medAbstractJob::MED_JOB_EXIT_FAILURE;

    ImageType::Pointer leftSideInput = this->CastImage(this->input1());
    ImageType::Pointer rightSideInput = this->CastImage(this->input2());

    typedef itk::SubtractImageFilter<ImageType, ImageType, ImageType> FilterType;
    typename FilterType::Pointer filter = FilterType::New();
    m_filter = filter;

    filter->SetInput1(leftSideInput);
    filter->SetInput2(rightSideInput);

    itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
    callback->SetClientData((void*)this);
    callback->SetCallback(medItkSubtractImageProcess::eventCallback);
    filter->AddObserver(itk::ProgressEvent(), callback);

    try
    {
        filter->Update();
    }
    catch(itk::ProcessAborted &)
    {
        return medAbstractJob::MED_JOB_EXIT_CANCELLED;
    }

    medAbstractImageData *out= qobject_cast<medAbstractImageData *>(medAbstractDataFactory::instance()->create("itkDataImageDouble3"));
    out->setData(filter->GetOutput());
    this->setOutput(out);
    return medAbstractJob::MED_JOB_EXIT_SUCCESS;
}

void medItkSubtractImageProcess::cancel()
{
    if(this->isRunning() && m_filter.IsNotNull())
    {
       m_filter->AbortGenerateDataOn();
    }
}
