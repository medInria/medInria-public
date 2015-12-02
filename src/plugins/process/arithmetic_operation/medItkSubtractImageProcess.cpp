/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkSubtractImageProcess.h>

#include <dtkLog>

#include <itkImage.h>
#include <itkSubtractImageFilter.h>
#include <itkCommand.h>

#include <medAbstractImageData.h>
#include <medAbstractDataFactory.h>
#include <medCore.h>


medItkSubtractImageProcess::medItkSubtractImageProcess(QObject *parent)
    : medAbstractSubtractImageProcess(parent)

{
    m_filter = NULL;
}

medItkSubtractImageProcess::~medItkSubtractImageProcess()
{

}

QString medItkSubtractImageProcess::caption() const
{
    return "Itk add images process";
}

QString medItkSubtractImageProcess::description() const
{
    return "Use itk::SubtractImageFilter to perform the addition of tow images.";
}

medAbstractJob::medJobExitStatus medItkSubtractImageProcess::run()
{
    medAbstractJob::medJobExitStatus jobExitSatus = medAbstractJob::MED_JOB_EXIT_FAILURE;

    if(this->input1() && this->input2())
    {
        QString id =  this->input1()->identifier();

        if ( id == "itkDataImageChar3" )
        {
            jobExitSatus = this->_run<char>();
        }
        else if ( id == "itkDataImageUChar3" )
        {
            jobExitSatus = this->_run<unsigned char>();
        }
        else if ( id == "itkDataImageShort3" )
        {
            jobExitSatus = this->_run<short>();
        }
        else if ( id == "itkDataImageUShort3" )
        {
            jobExitSatus = this->_run<unsigned short>();
        }
        else if ( id == "itkDataImageInt3" )
        {
            jobExitSatus = this->_run<int>();
        }
        else if ( id == "itkDataImageUInt3" )
        {
            jobExitSatus = this->_run<unsigned int>();
        }
        else if ( id == "itkDataImageLong3" )
        {
            jobExitSatus = this->_run<long>();
        }
        else if ( id== "itkDataImageULong3" )
        {
            jobExitSatus = this->_run<unsigned long>();
        }
        else if ( id == "itkDataImageFloat3" )
        {
            jobExitSatus = this->_run<float>();
        }
        else if ( id == "itkDataImageDouble3" )
        {
            jobExitSatus = this->_run<double>();
        }
    }
    return jobExitSatus;
}

template <class inputType>
medAbstractJob::medJobExitStatus medItkSubtractImageProcess::_run()
{
    typedef itk::Image<inputType, 3> ImageType;

    typename ImageType::Pointer in1 = dynamic_cast<ImageType *>((itk::Object*)(this->input1()->data()));
    typename ImageType::Pointer in2 = dynamic_cast<ImageType *>((itk::Object*)(this->input2()->data()));

    if(in1.IsNotNull() && in2.IsNotNull())
    {
        typedef itk::SubtractImageFilter<ImageType, ImageType, ImageType> FilterType;
        typename FilterType::Pointer filter = FilterType::New();
        m_filter = filter;

        filter->SetInput1(in1);
        filter->SetInput2(in2);

        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData((void*)this);
        callback->SetCallback(medItkSubtractImageProcess::eventCallback);
        filter->AddObserver(itk::ProgressEvent(), callback);

        try
        {
            filter->Update();
        }
        catch(itk::ProcessAborted &e)
        {
            return medAbstractJob::MED_JOB_EXIT_CANCELLED;
        }
        medAbstractImageData *out= qobject_cast<medAbstractImageData *>(medAbstractDataFactory::instance()->create("itkDataImageFloat3"));
        out->setData(filter->GetOutput());
        this->setOutput(out);
        return medAbstractJob::MED_JOB_EXIT_SUCCES;
    }
    return medAbstractJob::MED_JOB_EXIT_FAILURE;
}

void medItkSubtractImageProcess::cancel()
{
    if(this->isRunning() && m_filter.IsNotNull())
    {
       m_filter->AbortGenerateDataOn();
    }
}
