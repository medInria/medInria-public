#include <medItkNormalizeProcess.h>

#include <medItkWrapper.h>

#include <itkNormalizeImageFilter.h>

#include <medItkSubtractImageFilter.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkImage.h>
#include <itkCommand.h>
#include <itkExceptionObject.h>

#include <medMetaDataKeys.h>
#include <medAbstractDataFactory.h>

#include <medDoubleParameter.h>

template <>
class PrototypeInternal2<itk::NormalizeImageFilter>
{
    public:
    template <class T>
    int update(medItkFiltersProcessBase* process)
    {
        typedef itk::Image <T, 3> ImageType;
        typedef itk::NormalizeImageFilter<ImageType, ImageType> NormalizeImageFilterType;
        typename NormalizeImageFilterType::Pointer normalizeFilter = NormalizeImageFilterType::New();

        if (!process)
            return EXIT_FAILURE;

        normalizeFilter->SetInput (dynamic_cast<ImageType *> ( (itk::Object*) process->input<medAbstractData>(0)->data()));

        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) process );
        process->setCallBack(callback);

        normalizeFilter->AddObserver ( itk::ProgressEvent(), callback );

        try
        {
            normalizeFilter->Update();
        }
        catch (itk::ExceptionObject err)
        {
            resultString = err.GetDescription();
            return EXIT_FAILURE;
        }

        process->output<medAbstractData>(0)->setData ( normalizeFilter->GetOutput() );

        //Set output description metadata
        QString newSeriesDescription = process->input<medAbstractData>(0)->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " Normalize filter";

        process->output<medAbstractData>(0)->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );

        return EXIT_SUCCESS;
    }

    QString resultString;
};





medItkNormalizeProcess::medItkNormalizeProcess(medItkFiltersProcessBase *parent):
    medItkFiltersProcessBase(parent)
{
    this->setFilter(this);

    this->setDescription(tr("ITK Normalize filter"));

}

medItkNormalizeProcess::~medItkNormalizeProcess()
{
}

//-------------------------------------------------------------------------------------------

bool medItkNormalizeProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("medItkNormalizeProcess", createmedItkNormalizeProcess);
}

//-------------------------------------------------------------------------------------------

dtkAbstractProcess * createmedItkNormalizeProcess()
{
    qDebug() << "createmedItkNormalizeProcess";
    medItkFiltersProcessBase* baseProcess = new medItkNormalizeProcess;
    medItkWrapper* aWrapper = new TemplateWrapper2<itk::NormalizeImageFilter>(baseProcess);
    baseProcess->setWrapper(aWrapper);
    return baseProcess;
}
