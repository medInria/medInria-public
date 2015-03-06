#include <medItkInvertProcess.h>

#include <medItkWrapper.h>

#include <itkInvertIntensityImageFilter.h>

#include <medItkSubtractImageFilter.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkImage.h>
#include <itkCommand.h>
#include <itkExceptionObject.h>

#include <medMetaDataKeys.h>
#include <medAbstractDataFactory.h>

#include <medDoubleParameter.h>

template <>
class PrototypeInternal2<itk::InvertIntensityImageFilter>
{
    public:
    template <class T>
    int update(medItkFiltersProcessBase* process)
    {
        typedef itk::Image <T, 3> ImageType;
        typedef itk::InvertIntensityImageFilter<ImageType, ImageType> InvertFilterType;
        typename InvertFilterType::Pointer invertFilter = InvertFilterType::New();

        if (!process)
            return EXIT_FAILURE;

        invertFilter->SetInput (dynamic_cast<ImageType *> ( (itk::Object*) process->input<medAbstractData>(0)->data()));

        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) process );
        process->setCallBack(callback);

        invertFilter->AddObserver ( itk::ProgressEvent(), callback );

        try
        {
            invertFilter->Update();
        }
        catch (itk::ExceptionObject err)
        {
            resultString = err.GetDescription();
            return EXIT_FAILURE;
        }

        process->output<medAbstractData>(0)->setData ( invertFilter->GetOutput() );

        //Set output description metadata
        QString newSeriesDescription = process->input<medAbstractData>(0)->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " Invert filter";

        process->output<medAbstractData>(0)->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );

        return EXIT_SUCCESS;
    }

    QString resultString;
};





medItkInvertProcess::medItkInvertProcess(medItkFiltersProcessBase *parent):
    medItkFiltersProcessBase(parent)
{
    this->setFilter(this);

    this->setDescription(tr("ITK Invert filter"));

}

medItkInvertProcess::~medItkInvertProcess()
{
}

//-------------------------------------------------------------------------------------------

bool medItkInvertProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("medItkInvertProcess", createmedItkInvertProcess);
}

//-------------------------------------------------------------------------------------------

dtkAbstractProcess * createmedItkInvertProcess()
{
    qDebug() << "createmedItkInvertProcess";
    medItkFiltersProcessBase* baseProcess = new medItkInvertProcess;
    medItkWrapper* aWrapper = new TemplateWrapper2<itk::InvertIntensityImageFilter>(baseProcess);
    baseProcess->setWrapper(aWrapper);
    return baseProcess;
}
