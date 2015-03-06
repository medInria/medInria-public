#include <medItkDivideProcess.h>

#include <medItkWrapper.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkImage.h>
#include <itkCommand.h>
#include <itkExceptionObject.h>

#include <medMetaDataKeys.h>
#include <medAbstractDataFactory.h>

#include <medDoubleParameter.h>

#include <itkDivideImageFilter.h>

struct medItkDivideProcessPrivate
{
    medDoubleParameter* divideFactor;
};

class medItkDivideProcessDerived : public medItkDivideProcess
{
public:
    medItkDivideProcessDerived(medItkFiltersProcessBase* parent = NULL) : medItkDivideProcess(parent) { }
    medItkDivideProcessPrivate* internal()
    {
        return d;
    }
};

template <>
class PrototypeInternal3<itk::DivideImageFilter>
{
    public:
    template <class T>
    int update(medItkFiltersProcessBase* base)
    {
        typedef itk::Image< T, 3 > ImageType;
        typedef itk::DivideImageFilter< ImageType, itk::Image<double, ImageType::ImageDimension>, ImageType > DivideType;
        typename DivideType::Pointer divideFilter = DivideType::New();

        medItkDivideProcessDerived* process = dynamic_cast<medItkDivideProcessDerived*>(base);

        if (!process)
            return EXIT_FAILURE;

        divideFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( process->input<medAbstractData>(0)->data() ) ) );
        divideFilter->SetConstant(process->internal()->divideFactor->value());

        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) process );
        process->setCallBack(callback);

        divideFilter->AddObserver ( itk::ProgressEvent(), callback );
        divideFilter->Update();

        process->output<medAbstractData>(0)->setData ( divideFilter->GetOutput() );

        QString newSeriesDescription = process->input<medAbstractData>(0)->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " Divide filter (" + QString::number(process->internal()->divideFactor->value()) + ")";
        process->output<medAbstractData>(0)->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );

        return EXIT_SUCCESS;
    }

    QString resultString;
};





medItkDivideProcess::medItkDivideProcess(medItkFiltersProcessBase *parent):
    medItkFiltersProcessBase(parent), d(new medItkDivideProcessPrivate)
{
    this->setFilter(this);

    this->setDescription(tr("ITK divide filter"));

    //TODO MJB - Default settings
    d->divideFactor = new medDoubleParameter("Divide Factor");
    d->divideFactor->setRange(0,10);
    d->divideFactor->setValue(1.0);

    this->accessParameters() << d->divideFactor;
}

medItkDivideProcess::~medItkDivideProcess()
{
}

//-------------------------------------------------------------------------------------------

bool medItkDivideProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("medItkDivideProcess", createmedItkDivideProcess);
}

//-------------------------------------------------------------------------------------------

dtkAbstractProcess * createmedItkDivideProcess()
{
    medItkFiltersProcessBase* baseProcess = new medItkDivideProcessDerived;
    medItkWrapper* aWrapper = new TemplateWrapper3<itk::DivideImageFilter>(baseProcess);
    baseProcess->setWrapper(aWrapper);
    return baseProcess;
}
