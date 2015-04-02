#include <medItkMultiplyProcess.h>

#include <medItkWrapper.h>

#include <itkMultiplyImageFilter.h>

#include <medItkSubtractImageFilter.h>
#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkImage.h>
#include <itkCommand.h>
#include <itkExceptionObject.h>

#include <medMetaDataKeys.h>
#include <medAbstractDataFactory.h>

#include <medDoubleParameter.h>

struct medItkMultiplyProcessPrivate
{
    medDoubleParameter* multiplyFactor;
};

class medItkMultiplyProcessDerived : public medItkMultiplyProcess
{
public:
    medItkMultiplyProcessDerived(medItkFiltersProcessBase* parent = NULL) : medItkMultiplyProcess(parent) { }
    medItkMultiplyProcessPrivate* internal()
    {
        return d;
    }
};

template <>
class PrototypeInternal3<itk::MultiplyImageFilter>
{
    public:
    template <class T>
    int update(medItkFiltersProcessBase* base)
    {
        typedef itk::Image <T, 3> ImageType;
        typedef itk::MultiplyImageFilter<ImageType, itk::Image<double, ImageType::ImageDimension>, ImageType> MultiplyFilterType;
        typename MultiplyFilterType::Pointer multiplyFilter = MultiplyFilterType::New();

        medItkMultiplyProcessDerived* process = dynamic_cast<medItkMultiplyProcessDerived*>(base);

        if (!process)
            return EXIT_FAILURE;

        multiplyFilter->SetInput (dynamic_cast<ImageType *> ( (itk::Object*) process->input<medAbstractData>(0)->data()));
        multiplyFilter->SetConstant(process->internal()->multiplyFactor->value());

        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) process );
        process->setCallBack(callback);

        multiplyFilter->AddObserver ( itk::ProgressEvent(), callback );

        try
        {
            multiplyFilter->Update();
        }
        catch (itk::ExceptionObject err)
        {
            resultString = err.GetDescription();
            return EXIT_FAILURE;
        }

        process->output<medAbstractData>(0)->setData ( multiplyFilter->GetOutput() );

        //Set output description metadata
        QString newSeriesDescription = process->input<medAbstractData>(0)->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " Multiply filter (" + QString::number(process->internal()->multiplyFactor->value()) + ")";

        process->output<medAbstractData>(0)->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );

        return EXIT_SUCCESS;
    }

    QString resultString;
};





medItkMultiplyProcess::medItkMultiplyProcess(medItkFiltersProcessBase *parent):
    medItkFiltersProcessBase(parent), d(new medItkMultiplyProcessPrivate)
{
    this->setFilter(this);

    this->setDescription(tr("ITK Multiply filter"));

    d->multiplyFactor = new medDoubleParameter("Multiply value", this);
    d->multiplyFactor->setRange(0,100);
    d->multiplyFactor->setValue(100.0);

    this->accessParameters() << d->multiplyFactor;

}

medItkMultiplyProcess::~medItkMultiplyProcess()
{
}

//-------------------------------------------------------------------------------------------

bool medItkMultiplyProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("medItkMultiplyProcess", createmedItkMultiplyProcess);
}

//-------------------------------------------------------------------------------------------

dtkAbstractProcess * createmedItkMultiplyProcess()
{
    qDebug() << "createmedItkMultiplyProcess";
    medItkFiltersProcessBase* baseProcess = new medItkMultiplyProcessDerived;
    medItkWrapper* aWrapper = new TemplateWrapper3<itk::MultiplyImageFilter>(baseProcess);
    baseProcess->setWrapper(aWrapper);
    return baseProcess;
}
