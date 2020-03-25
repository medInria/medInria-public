#include <itkDWIBrainMaskCalculatorProcess.h>

#include <dtkLog>

#include <itkImage.h>
#include <itkExtractImageFilter.h>
#include <itkGetAverageSliceImageFilter.h>
#include <itkMaskImageFilter.h>
#include <itkJoinSeriesImageFilter.h>
#include <itkBrainExtractionImageFilter.h>

#include <medAbstractImageData.h>
#include <medAbstractDataFactory.h>


itkDWIBrainMaskCalculatorProcess::itkDWIBrainMaskCalculatorProcess(QObject *parent)
    : medAbstractDWIMaskingProcess(parent)

{
    m_offset = new medIntParameter("4d_offset_value", this);
    m_offset->setCaption("4D image offset");
    m_offset->setDescription("4D image offset to remove B0 images");
    m_offset->setValue(1);
}

itkDWIBrainMaskCalculatorProcess::~itkDWIBrainMaskCalculatorProcess()
{
}

void itkDWIBrainMaskCalculatorProcess::setInput(medAbstractImageData* data)
{
    this->medAbstractDWIMaskingProcess::setInput(data);

    m_offset->setRange(0,data->tDimension() - 1);
}

QString itkDWIBrainMaskCalculatorProcess::caption() const
{
    return "DWI brain masking";
}

QString itkDWIBrainMaskCalculatorProcess::description() const
{
    return "Use non B0 images to extract a brain mask";
}

medIntParameter *itkDWIBrainMaskCalculatorProcess::offset() const
{
    return m_offset;
}

medAbstractJob::medJobExitStatus itkDWIBrainMaskCalculatorProcess::run()
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
medAbstractJob::medJobExitStatus itkDWIBrainMaskCalculatorProcess::_run()
{
    typedef itk::Image<inputType, 4> ImageType;
    typedef itk::Image<inputType, 3> Image3DType;
    typedef itk::Image<unsigned char, 3> MaskImageType;

    typename ImageType::Pointer inData = dynamic_cast<ImageType *>((itk::Object*)(this->input()->data()));
    typename ImageType::Pointer inImage = dynamic_cast<ImageType *>((itk::Object*)(this->input()->data()));

    if (inData.IsNull())
        return medAbstractJob::MED_JOB_EXIT_FAILURE;

    try
    {
        // first, average all images except b0
        {
            typedef itk::ExtractImageFilter<ImageType, ImageType> Extract4DFilterType;
            typename ImageType::RegionType region = inData->GetLargestPossibleRegion();
            typename ImageType::SizeType     size = region.GetSize();
            typename ImageType::IndexType   index = region.GetIndex();
            size[3] -= m_offset->value();
            index[3] = m_offset->value();
            region.SetSize  (size);
            region.SetIndex (index);

            typename Extract4DFilterType::Pointer extractor = Extract4DFilterType::New();
            extractor->SetInput (inData);
            extractor->SetExtractionRegion (region);
            extractor->SetDirectionCollapseToGuess();

            extractor->Update();

            inData = extractor->GetOutput();
            inData->DisconnectPipeline();
        }

        {
            typedef itk::GetAverageSliceImageFilter<ImageType, ImageType> FilterType;
            typename FilterType::Pointer filter = FilterType::New();
            filter->SetInput (inData);
            filter->SetAveragedOutDimension (3);

            filter->Update();

            inData = filter->GetOutput();
            inData->DisconnectPipeline();
            // Correct bug in ITK accumulate filter that is not keeping direction matrix
            inData->SetDirection(inImage->GetDirection());
        }

        typename Image3DType::Pointer auxImage = nullptr;
        {
            typedef itk::ExtractImageFilter<ImageType, Image3DType> ExtractFilterType;
            typename ImageType::RegionType region = inData->GetLargestPossibleRegion();
            typename ImageType::SizeType     size = region.GetSize();
            typename ImageType::IndexType index = region.GetIndex();
            size[3] = 0;
            index[3] = 0;
            region.SetSize  (size);
            region.SetIndex (index);

            typename ExtractFilterType::Pointer extractor = ExtractFilterType::New();
            extractor->SetInput (inData);
            extractor->SetExtractionRegion (region);
            extractor->SetDirectionCollapseToGuess();

            extractor->Update();

            auxImage = extractor->GetOutput();
            auxImage->DisconnectPipeline();
        }

        // now, extract brain mask
        typename MaskImageType::Pointer maskImage = nullptr;
        {
            typedef itk::BrainExtractionImageFilter<Image3DType, MaskImageType> BrainExtractionFilterType;
            typename BrainExtractionFilterType::Pointer extractor = BrainExtractionFilterType::New();
            extractor->SetInput ( auxImage );

            extractor->Update();

            maskImage = extractor->GetOutput();
            maskImage->DisconnectPipeline();

            medAbstractImageData *out = qobject_cast<medAbstractImageData *>(medAbstractDataFactory::instance()->create("itkDataImageUChar3"));
            out->setData(maskImage);
            this->setOutput(out);
            return medAbstractJob::MED_JOB_EXIT_SUCCESS;
        }
    }
    catch (itk::ExceptionObject &e)
    {
        qDebug() << e.GetDescription();
        return medAbstractJob::MED_JOB_EXIT_FAILURE;
    }

    return medAbstractJob::MED_JOB_EXIT_FAILURE;
}

void itkDWIBrainMaskCalculatorProcess::cancel()
{
    // Not implemented yet
}
