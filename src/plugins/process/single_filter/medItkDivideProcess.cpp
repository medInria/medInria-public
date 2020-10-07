/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/



#include <medItkDivideProcess.h>

#include <dtkLog>

#include <itkImage.h>
#include <itkDivideImageFilter.h>
#include <itkCommand.h>

#include <medAbstractImageData.h>
#include <medAbstractDataFactory.h>

#include <medDoubleParameter.h>


medItkDivideProcess::medItkDivideProcess(QObject *parent)
    : medAbstractDivideFilterProcess(parent)

{
    m_filter = nullptr;
}

medItkDivideProcess::~medItkDivideProcess()
{

}

QString medItkDivideProcess::caption() const
{
    return "Divide by constant";
}

QString medItkDivideProcess::description() const
{
    return "Use ITK DivideImageFilter to divide an image by a constant.";
}

medAbstractJob::medJobExitStatus medItkDivideProcess::run()
{
    medAbstractJob::medJobExitStatus jobExitSatus = medAbstractJob::MED_JOB_EXIT_FAILURE;

    if(this->input())
    {
        QString id =  this->input()->identifier();

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
medAbstractJob::medJobExitStatus medItkDivideProcess::_run()
{
    typedef itk::Image<inputType, 3> ImageType;

    typename ImageType::Pointer in1 = dynamic_cast<ImageType *>((itk::Object*)(this->input()->data()));

    double factor = this->doubleParameter()->value();

    if(in1.IsNotNull())
    {
        typedef itk::DivideImageFilter< ImageType, itk::Image<double, ImageType::ImageDimension>, ImageType >  FilterType;
        typename FilterType::Pointer filter = FilterType::New();
        m_filter = filter;

        filter->SetInput(in1);
        filter->SetConstant(factor);

        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData((void*)this);
        callback->SetCallback(medItkDivideProcess::eventCallback);
        filter->AddObserver(itk::ProgressEvent(), callback);

        try
        {
            filter->Update();
        }
        catch(itk::ProcessAborted &)
        {
            return medAbstractJob::MED_JOB_EXIT_CANCELLED;
        }

        medAbstractImageData *out= qobject_cast<medAbstractImageData *>(medAbstractDataFactory::instance()->create(this->input()->identifier()));
        out->setData(filter->GetOutput());
        this->setOutput(out);
        return medAbstractJob::MED_JOB_EXIT_SUCCESS;
    }
    return medAbstractJob::MED_JOB_EXIT_FAILURE;


}

void medItkDivideProcess::cancel()
{
    if(this->isRunning() && m_filter.IsNotNull())
    {
        m_filter->AbortGenerateDataOn();
    }
}
