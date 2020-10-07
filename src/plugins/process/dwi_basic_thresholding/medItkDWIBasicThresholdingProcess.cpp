/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkDWIBasicThresholdingProcess.h>

#include <dtkLog>

#include <itkImage.h>
#include <itkExtractImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkCommand.h>

#include <medAbstractImageData.h>
#include <medAbstractDataFactory.h>


medItkDWIBasicThresholdingProcess::medItkDWIBasicThresholdingProcess(QObject *parent)
    : medAbstractDWIMaskingProcess(parent)
{
    m_extractfilter = 0;
    m_thresholdfilter = 0;

    m_b0VolumeNumber = new medIntParameter("b0_volume_number", this);
    m_b0VolumeNumber->setCaption("B0 volume index");
    m_b0VolumeNumber->setDescription("B0 volume index to threshold");
    m_b0VolumeNumber->setValue(0);

    m_thresholdValue = new medIntParameter("threshold_value", this);
    m_thresholdValue->setCaption("Lower threshold value");
    m_thresholdValue->setDescription("Lower threshold value");
    m_thresholdValue->setRange(0,5000);
    m_thresholdValue->setValue(100);
}

medItkDWIBasicThresholdingProcess::~medItkDWIBasicThresholdingProcess()
{
}

medIntParameter *medItkDWIBasicThresholdingProcess::b0VolumeNumberParameter() const
{
    return m_b0VolumeNumber;
}

medIntParameter *medItkDWIBasicThresholdingProcess::thresholdValueParameter() const
{
    return m_thresholdValue;
}


void medItkDWIBasicThresholdingProcess::setInput(medAbstractImageData* data)
{
    this->medAbstractDWIMaskingProcess::setInput(data);

    m_b0VolumeNumber->setRange(0,data->tDimension() - 1);
}

QString medItkDWIBasicThresholdingProcess::caption() const
{
    return "DWI basic thresholding";
}

QString medItkDWIBasicThresholdingProcess::description() const
{
    return "Use ITK BinaryThresholdImageFilter to extract a binary mask";
}

medAbstractJob::medJobExitStatus medItkDWIBasicThresholdingProcess::run()
{
    medAbstractJob::medJobExitStatus jobExitSatus = medAbstractJob::MED_JOB_EXIT_FAILURE;

    if(this->input())
    {
        QString id =  this->input()->identifier();

        if ( id == "itkDataImageChar4" )
        {
            jobExitSatus = this->_run<char>();
        }
        else if ( id == "itkDataImageUChar4" )
        {
            jobExitSatus = this->_run<unsigned char>();
        }
        else if ( id == "itkDataImageShort4" )
        {
            jobExitSatus = this->_run<short>();
        }
        else if ( id == "itkDataImageUShort4" )
        {
            jobExitSatus = this->_run<unsigned short>();
        }
        else if ( id == "itkDataImageInt4" )
        {
            jobExitSatus = this->_run<int>();
        }
        else if ( id == "itkDataImageUInt4" )
        {
            jobExitSatus = this->_run<unsigned int>();
        }
        else if ( id == "itkDataImageLong4" )
        {
            jobExitSatus = this->_run<long>();
        }
        else if ( id== "itkDataImageULong4" )
        {
            jobExitSatus = this->_run<unsigned long>();
        }
        else if ( id == "itkDataImageFloat4" )
        {
            jobExitSatus = this->_run<float>();
        }
        else if ( id == "itkDataImageDouble4" )
        {
            jobExitSatus = this->_run<double>();
        }
    }

    return jobExitSatus;
}

template <class inputType>
medAbstractJob::medJobExitStatus medItkDWIBasicThresholdingProcess::_run()
{
    typedef itk::Image<inputType, 4> ImageType;
    typedef itk::Image<inputType, 3> Image3DType;

    typename ImageType::Pointer inData = dynamic_cast<ImageType *>((itk::Object*)(this->input()->data()));

    if(inData.IsNotNull())
    {
        typedef itk::ExtractImageFilter <ImageType, Image3DType> FilterType;

        typename FilterType::Pointer filter = FilterType::New();
        filter->SetDirectionCollapseToGuess();
        typename ImageType::RegionType extractRegion = inData->GetLargestPossibleRegion();
        extractRegion.SetIndex(3,m_b0VolumeNumber->value());
        extractRegion.SetSize(3,0);

        filter->SetExtractionRegion(extractRegion);
        m_extractfilter = filter;
        filter->SetInput(0,inData);

        typedef itk::Image <unsigned char, 3> MaskImageType;
        typedef itk::BinaryThresholdImageFilter <Image3DType, MaskImageType> ThresholdFilterType;
        typename ThresholdFilterType::Pointer thr_filter = ThresholdFilterType::New();

        thr_filter->SetLowerThreshold(m_thresholdValue->value());
        thr_filter->SetInsideValue(1);

        m_thresholdfilter = thr_filter;
        thr_filter->SetInput(0,filter->GetOutput());

        try
        {
            m_thresholdfilter->Update();
        }
        catch(itk::ProcessAborted &)
        {
            return medAbstractJob::MED_JOB_EXIT_CANCELLED;
        }

        medAbstractImageData *out = qobject_cast<medAbstractImageData *>(medAbstractDataFactory::instance()->create("itkDataImageUChar3"));
        out->setData(thr_filter->GetOutput());
        this->setOutput(out);
        return medAbstractJob::MED_JOB_EXIT_SUCCESS;
    }

    return medAbstractJob::MED_JOB_EXIT_FAILURE;
}

void medItkDWIBasicThresholdingProcess::cancel()
{
    if(this->isRunning() && m_thresholdfilter.IsNotNull())
    {
        m_thresholdfilter->AbortGenerateDataOn();
    }
}
