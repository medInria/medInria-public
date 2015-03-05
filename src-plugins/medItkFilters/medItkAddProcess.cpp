#include <medItkAddProcess.h>

#include <medItkWrapper.h>

#include <itkAddImageFilter.h>

#include <medItkSubtractImageFilter.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkImage.h>
#include <itkCommand.h>
#include <itkExceptionObject.h>

#include <medMetaDataKeys.h>
#include <medAbstractDataFactory.h>

#include <medDoubleParameter.h>

struct medItkAddProcessPrivate
{
    medDoubleParameter* addValue;
};

class medItkAddProcessDerived : public medItkAddProcess
{
public:
    medItkAddProcessDerived(medItkFiltersProcessBase* parent = NULL) : medItkAddProcess(parent) { }
    medItkAddProcessPrivate* internal()
    {
        return d;
    }
};

template <>
class PrototypeInternal<itk::AddImageFilter>
{
    public:
    template <class T>
    int update(medItkFiltersProcessBase* base)
    {
        typedef itk::Image <T, 3> ImageType;
        typedef itk::AddImageFilter<ImageType, itk::Image<double, ImageType::ImageDimension>, ImageType> AddFilterType;
        typename AddFilterType::Pointer addFilter = AddFilterType::New();

        medItkAddProcessDerived* process = dynamic_cast<medItkAddProcessDerived*>(base);

        if (!process)
            return EXIT_FAILURE;

        addFilter->SetInput (dynamic_cast<ImageType *> ( (itk::Object*) process->input<medAbstractData>(0)->data()));
        addFilter->SetConstant(process->internal()->addValue->value());

        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) process );
        process->setCallBack(callback);

        addFilter->AddObserver ( itk::ProgressEvent(), callback );

        try
        {
            addFilter->Update();
        }
        catch (itk::ExceptionObject err)
        {
            resultString = err.GetDescription();
            return EXIT_FAILURE;
        }

        process->output<medAbstractData>(0)->setData ( addFilter->GetOutput() );

        //Set output description metadata
        QString newSeriesDescription = process->input<medAbstractData>(0)->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " add filter (" + QString::number(process->internal()->addValue->value()) + ")";

        process->output<medAbstractData>(0)->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );

        return EXIT_SUCCESS;
    }

    QString resultString;
};





medItkAddProcess::medItkAddProcess(medItkFiltersProcessBase *parent):
    medItkFiltersProcessBase(parent), d(new medItkAddProcessPrivate)
{
    this->setFilter(this);

    this->setDescription(tr("ITK add filter"));

    d->addValue = new medDoubleParameter("Add value", this);
    d->addValue->setRange(0,100);
    d->addValue->setValue(100.0);

    this->accessParameters() << d->addValue;

}

medItkAddProcess::~medItkAddProcess()
{
}

//-------------------------------------------------------------------------------------------

bool medItkAddProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("medItkAddProcess", createmedItkAddProcess);
}

//-------------------------------------------------------------------------------------------

dtkAbstractProcess * createmedItkAddProcess()
{
    qDebug() << "createmedItkAddProcess";
    medItkFiltersProcessBase* baseProcess = new medItkAddProcessDerived;
    medItkWrapper* aWrapper = new TemplateWrapper<itk::AddImageFilter>(baseProcess);
    baseProcess->setWrapper(aWrapper);
    return baseProcess;
}
