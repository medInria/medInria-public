/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkMaskImageProcess.h>

#include <dtkLog>

#include <itkImage.h>
#include <itkMaskImageFilter.h>
#include <itkBinaryBallStructuringElement.h>
#include <itkCommand.h>

#include <medAbstractImageData.h>
#include <medAbstractDataFactory.h>
#include <medIntParameter.h>


medItkMaskImageProcess::medItkMaskImageProcess(QObject *parent)
    : medAbstractMaskImageProcess(parent)

{
    m_filter = nullptr;
}

medItkMaskImageProcess::~medItkMaskImageProcess()
{

}


QString medItkMaskImageProcess::caption() const
{
    return "Mask image";
}

QString medItkMaskImageProcess::description() const
{
    return "Use ITK MaskImageFilter to apply a mask to an image.";
}
medAbstractJob::medJobExitStatus medItkMaskImageProcess::run()
{
    medAbstractJob::medJobExitStatus jobExitSatus = medAbstractJob::MED_JOB_EXIT_FAILURE;

    if(this->input())
    {
        QString id =  this->input()->identifier();

        if ( id == "itkDataImageChar3" )
        {
            jobExitSatus = this->_run<char, 3>();
        }
        else if ( id == "itkDataImageUChar3" )
        {
            jobExitSatus = this->_run<unsigned char, 3>();
        }
        else if ( id == "itkDataImageShort3" )
        {
            jobExitSatus = this->_run<short, 3>();
        }
        else if ( id == "itkDataImageUShort3" )
        {
            jobExitSatus = this->_run<unsigned short, 3>();
        }
        else if ( id == "itkDataImageInt3" )
        {
            jobExitSatus = this->_run<int, 3>();
        }
        else if ( id == "itkDataImageUInt3" )
        {
            jobExitSatus = this->_run<unsigned int, 3>();
        }
        else if ( id == "itkDataImageLong3" )
        {
            jobExitSatus = this->_run<long, 3>();
        }
        else if ( id== "itkDataImageULong3" )
        {
            jobExitSatus = this->_run<unsigned long, 3>();
        }
        else if ( id == "itkDataImageFloat3" )
        {
            jobExitSatus = this->_run<float, 3>();
        }
        else if ( id == "itkDataImageDouble3" )
        {
            jobExitSatus = this->_run<double, 3>();
        }
        else if ( id == "itkDataImageChar4" )
        {
            jobExitSatus = this->_run<char, 4>();
        }
        else if ( id == "itkDataImageUChar4" )
        {
            jobExitSatus = this->_run<unsigned char, 4>();
        }
        else if ( id == "itkDataImageShort4" )
        {
            jobExitSatus = this->_run<short, 4>();
        }
        else if ( id == "itkDataImageUShort4" )
        {
            jobExitSatus = this->_run<unsigned short, 4>();
        }
        else if ( id == "itkDataImageInt4" )
        {
            jobExitSatus = this->_run<int, 4>();
        }
        else if ( id == "itkDataImageUInt4" )
        {
            jobExitSatus = this->_run<unsigned int, 4>();
        }
        else if ( id == "itkDataImageLong4" )
        {
            jobExitSatus = this->_run<long, 4>();
        }
        else if ( id == "itkDataImageULong4" )
        {
            jobExitSatus = this->_run<unsigned long, 4>();
        }
        else if ( id == "itkDataImageFloat4" )
        {
            jobExitSatus = this->_run<float, 4>();
        }
        else if ( id == "itkDataImageDouble4" )
        {
            jobExitSatus = this->_run<double, 4>();
        }
    }
    return jobExitSatus;
}

template <class inputType, unsigned int Dimension>
medAbstractJob::medJobExitStatus medItkMaskImageProcess::_run()
{
    typedef itk::Image<inputType, Dimension> ImageType;
    typedef itk::Image<unsigned char, Dimension> MaskType;
    typename MaskType::Pointer ma;

    typename ImageType::Pointer in = dynamic_cast<ImageType *>((itk::Object*)(this->input()->data()));
    if(this->mask()->Dimension() == Dimension - 1)
    {
        typedef itk::Image<unsigned char, Dimension - 1> SmallMaskType;
        typename SmallMaskType::Pointer sma = dynamic_cast<SmallMaskType *>((itk::Object*)(this->mask()->data()));
        ma = MaskType::New();
        ma->Initialize();
        ma->SetRegions(in->GetLargestPossibleRegion());
        ma->SetOrigin(in->GetOrigin());
        ma->SetSpacing(in->GetSpacing());
        ma->SetDirection(in->GetDirection());
        ma->Allocate();
        typedef itk::ImageRegionIterator<MaskType> MaskIteratorType;
        typedef itk::ImageRegionConstIterator<SmallMaskType> SmallMaskIteratorType;
        MaskIteratorType completeMaskIt(ma, ma->GetLargestPossibleRegion());
        for(int i = 0; i < static_cast<int>(ma->GetLargestPossibleRegion().GetSize()[Dimension - 1]); ++i)
        {
            SmallMaskIteratorType smallMaskIt(sma, sma->GetLargestPossibleRegion());
            while(!smallMaskIt.IsAtEnd())
            {
                completeMaskIt.Set(smallMaskIt.Get());
                ++completeMaskIt;
                ++smallMaskIt;
            }
        }
    }
    else
    {
        ma = dynamic_cast<MaskType *>((itk::Object*)(this->mask()->data()));
    }

    if(in.IsNotNull() && ma.IsNotNull())
    {
        typedef itk::MaskImageFilter<ImageType, MaskType, ImageType> FilterType;
        typename FilterType::Pointer filter = FilterType::New();
        m_filter = filter;

        filter->SetInput(in);
        filter->SetMaskImage(ma);

        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData((void*)this);
        callback->SetCallback(medItkMaskImageProcess::eventCallback);
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

void medItkMaskImageProcess::cancel()
{
    if(this->isRunning() && m_filter.IsNotNull())
    {
        m_filter->AbortGenerateDataOn();
    }
}
