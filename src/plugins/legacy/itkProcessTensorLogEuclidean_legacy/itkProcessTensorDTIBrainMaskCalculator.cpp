#include "itkProcessTensorDTIBrainMaskCalculator.h"

#include <dtkCore/dtkSmartPointer.h>
#include <dtkLog/dtkLog.h>
#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkImage.h>
#include <itkExtractImageFilter.h>
#include <itkGetAverageSliceImageFilter.h>
#include <itkMaskImageFilter.h>
#include <itkJoinSeriesImageFilter.h>
#include <itkBrainExtractionImageFilter.h>

class itkProcessTensorDTIBrainMaskCalculatorPrivate
{
public:
    template <class TImage>
    int itkProcessTensorDTIBrainMaskCalculatorImpl(TImage *image);

    dtkSmartPointer<medAbstractData> output;
    dtkSmartPointer<medAbstractData> input;
    int offset;
};

template <class TImage>
int itkProcessTensorDTIBrainMaskCalculatorPrivate::itkProcessTensorDTIBrainMaskCalculatorImpl(TImage *inImage)
{
    if (!inImage)
        return -1;

    typedef TImage                                       ImageType;
    typedef itk::Image<typename ImageType::PixelType, 3> AuxImageType;
    typedef itk::Image<unsigned char, 3>                 MaskImageType;

    typename ImageType::Pointer image = inImage;
    try
    {
        // first, average all images except b0
        {
            typedef itk::ExtractImageFilter<ImageType, ImageType> Extract4DFilterType;
            typename ImageType::RegionType region = image->GetLargestPossibleRegion();
            typename ImageType::SizeType     size = region.GetSize();
            typename ImageType::IndexType   index = region.GetIndex();
            size[3] -= this->offset;
            index[3] = this->offset;
            region.SetSize  (size);
            region.SetIndex (index);

            typename Extract4DFilterType::Pointer extractor = Extract4DFilterType::New();
            extractor->SetInput (image);
            extractor->SetExtractionRegion (region);
            extractor->SetDirectionCollapseToGuess();

            extractor->Update();

            image = extractor->GetOutput();
            image->DisconnectPipeline();
        }

        {
            typedef itk::GetAverageSliceImageFilter<ImageType, ImageType> FilterType;
            typename FilterType::Pointer filter = FilterType::New();
            filter->SetInput (image);
            filter->SetAveragedOutDimension (3);

            filter->Update();

            image = filter->GetOutput();
            image->DisconnectPipeline();
            // Correct bug in ITK accumulate filter that is not keeping direction matrix
            image->SetDirection(inImage->GetDirection());
        }

        typename AuxImageType::Pointer auxImage = 0;
        {
            typedef itk::ExtractImageFilter<ImageType, AuxImageType> ExtractFilterType;
            typename ImageType::RegionType region = image->GetLargestPossibleRegion();
            typename ImageType::SizeType     size = region.GetSize();
            typename ImageType::IndexType   index = region.GetIndex();
            size[3] = 0;
            index[3] = 0;
            region.SetSize  (size);
            region.SetIndex (index);

            typename ExtractFilterType::Pointer extractor = ExtractFilterType::New();
            extractor->SetInput (image);
            extractor->SetExtractionRegion (region);
            extractor->SetDirectionCollapseToGuess();

            extractor->Update();

            auxImage = extractor->GetOutput();
            auxImage->DisconnectPipeline();
        }

        // now, extract brain mask
        typename MaskImageType::Pointer maskImage = 0;
        {
            typedef itk::BrainExtractionImageFilter<AuxImageType, MaskImageType> BrainExtractionFilterType;
            typename BrainExtractionFilterType::Pointer extractor = BrainExtractionFilterType::New();
            extractor->SetInput ( auxImage );

            extractor->Update();

            maskImage = extractor->GetOutput();
            maskImage->DisconnectPipeline();
        }

        // finally, apply mask to original 4D image
        {
            typedef itk::MaskImageFilter<AuxImageType, MaskImageType, AuxImageType>
                    MaskFilterType;

            typedef itk::ExtractImageFilter<ImageType, AuxImageType> ExtractFilterType;

            typedef itk::JoinSeriesImageFilter<AuxImageType, ImageType> JoinFilterType;

            typename JoinFilterType::Pointer joiner = JoinFilterType::New();

            unsigned int volumeCount = inImage->GetLargestPossibleRegion().GetSize()[3];

            for (unsigned int i=0; i<volumeCount; i++)
            {
                typename ImageType::SizeType size = inImage->GetLargestPossibleRegion().GetSize();
                size[3] = 0;
                typename ImageType::IndexType index = {{0,0,0,i}};

                typename ImageType::RegionType region;
                region.SetSize(size);
                region.SetIndex(index);

                typename ExtractFilterType::Pointer extractor = ExtractFilterType::New();
                extractor->SetExtractionRegion(region);
                extractor->SetDirectionCollapseToGuess();
                extractor->SetInput(inImage);

                typename MaskFilterType::Pointer masker = MaskFilterType::New();
                masker->SetInput1 (extractor->GetOutput());
                masker->SetInput2 (maskImage);

                masker->Update();

                joiner->PushBackInput (masker->GetOutput());
                masker->GetOutput()->DisconnectPipeline();
            }

            joiner->Update();

            if (this->output)
                this->output->setData(joiner->GetOutput());

            joiner->GetOutput()->DisconnectPipeline();
        }
    }
    catch (itk::ExceptionObject &e)
    {
        qDebug() << e.GetDescription();
        return -1;
    }

    return 0;
}

itkProcessTensorDTIBrainMaskCalculator::itkProcessTensorDTIBrainMaskCalculator() : d(new itkProcessTensorDTIBrainMaskCalculatorPrivate)
{
    d->input  = 0;
    d->output = 0;
    d->offset = 0;
}

itkProcessTensorDTIBrainMaskCalculator::~itkProcessTensorDTIBrainMaskCalculator()
{
    delete d;
    d = 0;
}

QString itkProcessTensorDTIBrainMaskCalculator::identifier(void) const
{
    return "itkProcessTensorDTIBrainMaskCalculator";
}

QString itkProcessTensorDTIBrainMaskCalculator::description(void) const
{
    return "itkProcessTensorDTIBrainMaskCalculator";
}

bool itkProcessTensorDTIBrainMaskCalculator::registered (void)
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkProcessTensorDTIBrainMaskCalculator", createItkProcessTensorDTIBrainMaskCalculator);
}

void itkProcessTensorDTIBrainMaskCalculator::setInputImage(medAbstractData *data)
{
    if (!data)
        return;

    d->input = data;

    d->output = medAbstractDataFactory::instance()->createSmartPointer(d->input->identifier());

    if (!d->output)
    {
        dtkError() << "cannot create object of type: " << d->input->identifier();
    }
}

void *itkProcessTensorDTIBrainMaskCalculator::data(void)
{
    if (d->output)
        return d->output->data();

    return NULL;
}

medAbstractData *itkProcessTensorDTIBrainMaskCalculator::output(void)
{
    return d->output;
}

void itkProcessTensorDTIBrainMaskCalculator::setParameter(double value, int channel)
{
    if (channel==0)
        d->offset = static_cast<int>(value);
    else
        dtkWarn() << "parameter id out of range: " << channel;
}

int itkProcessTensorDTIBrainMaskCalculator::update(void)
{
    if (!d->input) {
        dtkError() << "input must be set prior to calling update() function";
        return -1;
    }

    if (!d->output) {
        dtkError() << "output could not be created";
        return -1;
    }

    if (d->input->identifier()=="itkDataImageUChar4") {
        typedef itk::Image<unsigned char, 4> ImageType;
        ImageType::Pointer image = static_cast<ImageType*>(d->input->data());

        return d->itkProcessTensorDTIBrainMaskCalculatorImpl<ImageType>(image);
    }
    else if (d->input->identifier()=="itkDataImageChar4") {
        typedef itk::Image<char, 4> ImageType;
        ImageType::Pointer image = static_cast<ImageType*>(d->input->data());

        return d->itkProcessTensorDTIBrainMaskCalculatorImpl<ImageType>(image);
    }
    else if (d->input->identifier()=="itkDataImageUShort4") {
        typedef itk::Image<unsigned short, 4> ImageType;
        ImageType::Pointer image = static_cast<ImageType*>(d->input->data());

        return d->itkProcessTensorDTIBrainMaskCalculatorImpl<ImageType>(image);
    }
    else if (d->input->identifier()=="itkDataImageShort4") {
        typedef itk::Image<short, 4> ImageType;
        ImageType::Pointer image = static_cast<ImageType*>(d->input->data());

        return d->itkProcessTensorDTIBrainMaskCalculatorImpl<ImageType>(image);
    }
    else if (d->input->identifier()=="itkDataImageUInt4") {
        typedef itk::Image<unsigned int, 4> ImageType;
        ImageType::Pointer image = static_cast<ImageType*>(d->input->data());

        return d->itkProcessTensorDTIBrainMaskCalculatorImpl<ImageType>(image);
    }
    else if (d->input->identifier()=="itkDataImageInt4") {
        typedef itk::Image<int, 4> ImageType;
        ImageType::Pointer image = static_cast<ImageType*>(d->input->data());

        return d->itkProcessTensorDTIBrainMaskCalculatorImpl<ImageType>(image);
    }
    else if (d->input->identifier()=="itkDataImageULong4") {
        typedef itk::Image<unsigned long, 4> ImageType;
        ImageType::Pointer image = static_cast<ImageType*>(d->input->data());

        return d->itkProcessTensorDTIBrainMaskCalculatorImpl<ImageType>(image);
    }
    else if (d->input->identifier()=="itkDataImageLong4") {
        typedef itk::Image<long, 4> ImageType;
        ImageType::Pointer image = static_cast<ImageType*>(d->input->data());

        return d->itkProcessTensorDTIBrainMaskCalculatorImpl<ImageType>(image);
    }
    else if (d->input->identifier()=="itkDataImageFloat4") {
        typedef itk::Image<float, 4> ImageType;
        ImageType::Pointer image = static_cast<ImageType*>(d->input->data());

        return d->itkProcessTensorDTIBrainMaskCalculatorImpl<ImageType>(image);
    }
    else if (d->input->identifier()=="itkDataImageDouble4") {
        typedef itk::Image<double, 4> ImageType;
        ImageType::Pointer image = static_cast<ImageType*>(d->input->data());

        return d->itkProcessTensorDTIBrainMaskCalculatorImpl<ImageType>(image);
    }
    else
        dtkError() << "unsupported input type: " << d->input->identifier();

    return -1;
}

// /////////////////////////////////////////////////////////////////
// Process type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createItkProcessTensorDTIBrainMaskCalculator(void)
{
    return new itkProcessTensorDTIBrainMaskCalculator;
}
