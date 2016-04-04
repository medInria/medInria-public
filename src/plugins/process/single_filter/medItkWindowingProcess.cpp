/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkWindowingProcess.h>

#include <dtkLog>

#include <itkImage.h>
#include <itkIntensityWindowingImageFilter.h>
#include <itkCommand.h>

#include <medAbstractImageData.h>
#include <medAbstractDataFactory.h>

#include <medDoubleParameter.h>


medItkWindowingProcess::medItkWindowingProcess(QObject *parent)
    : medAbstractWindowingFilterProcess(parent)

{
    m_filter = NULL;
}

medItkWindowingProcess::~medItkWindowingProcess()
{

}

QString medItkWindowingProcess::caption() const
{
    return "Image windowing";
}

QString medItkWindowingProcess::description() const
{
    return "Use ITK IntensityWindowingImageFilter to apply a window to image intensities.";
}

medAbstractJob::medJobExitStatus medItkWindowingProcess::run()
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
medAbstractJob::medJobExitStatus medItkWindowingProcess::_run()
{
    typedef itk::Image<inputType, 3> ImageType;

    typename ImageType::Pointer in1 = dynamic_cast<ImageType *>((itk::Object*)(this->input()->data()));

    double minimumIntensityValue = this->minimumIntensityValue()->value();
    double maximumIntensityValue = this->maximumIntensityValue()->value();
    double minimumOutputIntensityValue = this->minimumOutputIntensityValue()->value();
    double maximumOutputIntensityValue = this->maximumOutputIntensityValue()->value();

    if(in1.IsNotNull())
    {
        typedef itk::IntensityWindowingImageFilter<ImageType, ImageType> FilterType;
        typename FilterType::Pointer filter = FilterType::New();
        m_filter = filter;


        filter->SetInput(in1);

        filter->SetWindowMinimum ( ( inputType ) minimumIntensityValue );
        filter->SetWindowMaximum ( ( inputType ) maximumIntensityValue );
        filter->SetOutputMinimum ( ( inputType ) minimumOutputIntensityValue );
        filter->SetOutputMaximum ( ( inputType ) maximumOutputIntensityValue );

        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData((void*)this);
        callback->SetCallback(medItkWindowingProcess::eventCallback);
        filter->AddObserver(itk::ProgressEvent(), callback);

        try
        {
            filter->Update();
        }
        catch(itk::ProcessAborted &e)
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

void medItkWindowingProcess::cancel()
{
    if(this->isRunning() && m_filter.IsNotNull())
    {
        m_filter->AbortGenerateDataOn();
    }
}
