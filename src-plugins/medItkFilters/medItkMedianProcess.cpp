#include <medItkMedianProcess.h>

#include <medItkWrapper.h>

#include <itkMedianImageFilter.h>

#include <medItkSubtractImageFilter.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkImage.h>
#include <itkCommand.h>
#include <itkExceptionObject.h>

#include <medMetaDataKeys.h>
#include <medAbstractDataFactory.h>

#include <medDoubleParameter.h>

template <>
class PrototypeInternal2<itk::MedianImageFilter>
{
    public:
    template <class T>
    int update(medItkFiltersProcessBase* process)
    {
        typedef itk::Image <T, 3> ImageType;
        typedef itk::MedianImageFilter<ImageType, ImageType> MedianImageFilterType;
        typename MedianImageFilterType::Pointer medianFilter = MedianImageFilterType::New();

        if (!process)
            return EXIT_FAILURE;

        medianFilter->SetInput (dynamic_cast<ImageType *> ( (itk::Object*) process->input<medAbstractData>(0)->data()));

        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) process );
        process->setCallBack(callback);

        medianFilter->AddObserver ( itk::ProgressEvent(), callback );

        try
        {
            medianFilter->Update();
        }
        catch (itk::ExceptionObject err)
        {
            resultString = err.GetDescription();
            return EXIT_FAILURE;
        }

        process->output<medAbstractData>(0)->setData ( medianFilter->GetOutput() );

        //Set output description metadata
        QString newSeriesDescription = process->input<medAbstractData>(0)->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " Median filter";

        process->output<medAbstractData>(0)->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );

        return EXIT_SUCCESS;
    }

    QString resultString;
};





medItkMedianProcess::medItkMedianProcess(medItkFiltersProcessBase *parent):
    medItkFiltersProcessBase(parent)
{
    this->setFilter(this);

    this->setDescription(tr("ITK Median filter"));

}

medItkMedianProcess::~medItkMedianProcess()
{
}

//-------------------------------------------------------------------------------------------

bool medItkMedianProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("medItkMedianProcess", createmedItkMedianProcess);
}

//-------------------------------------------------------------------------------------------

dtkAbstractProcess * createmedItkMedianProcess()
{
    qDebug() << "createmedItkMedianProcess";
    medItkFiltersProcessBase* baseProcess = new medItkMedianProcess;
    medItkWrapper* aWrapper = new TemplateWrapper2<itk::MedianImageFilter>(baseProcess);
    baseProcess->setWrapper(aWrapper);
    return baseProcess;
}
