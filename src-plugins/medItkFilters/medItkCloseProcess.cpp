#include <medItkCloseProcess.h>

#include <medItkWrapper.h>

#include <itkGrayscaleMorphologicalClosingImageFilter.h>
#include <itkBinaryBallStructuringElement.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkImage.h>
#include <itkCommand.h>
#include <itkExceptionObject.h>

#include <medMetaDataKeys.h>
#include <medAbstractDataFactory.h>

#include <medDoubleParameter.h>

struct medItkCloseProcessPrivate
{
    medDoubleParameter* radius;
};

class medItkCloseProcessDerived : public medItkCloseProcess
{
public:
    medItkCloseProcessDerived(medItkFiltersProcessBase* parent = NULL) : medItkCloseProcess(parent) { }
    medItkCloseProcessPrivate* internal()
    {
        return d;
    }
};

template <>
class PrototypeInternal<itk::GrayscaleMorphologicalClosingImageFilter>
{
    public:
    template <class T>
    int update(medItkFiltersProcessBase* base)
    {
        typedef itk::Image< T, 3 > ImageType;
        typedef itk::BinaryBallStructuringElement < T, 3> StructuringElementType;
        typedef itk::GrayscaleMorphologicalClosingImageFilter< ImageType, ImageType, StructuringElementType > CloseType;
        typename CloseType::Pointer closeFilter = CloseType::New();

        medItkCloseProcessDerived* process = dynamic_cast<medItkCloseProcessDerived*>(base);

        if (!process)
            return EXIT_FAILURE;

        StructuringElementType ball;
        ball.SetRadius(process->internal()->radius->value());
        ball.CreateStructuringElement();

        closeFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( process->input<medAbstractData>(0)->data() ) ) );

        closeFilter->SetKernel ( ball );
        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) process );
        process->setCallBack(callback);

        closeFilter->AddObserver ( itk::ProgressEvent(), callback );
        closeFilter->Update();

        process->output<medAbstractData>(0)->setData ( closeFilter->GetOutput() );

        QString newSeriesDescription = process->input<medAbstractData>(0)->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " Close filter (" + QString::number(process->internal()->radius->value()) + ")";
        process->output<medAbstractData>(0)->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );

        return EXIT_SUCCESS;
    }

    QString resultString;
};





medItkCloseProcess::medItkCloseProcess(medItkFiltersProcessBase *parent):
    medItkFiltersProcessBase(parent), d(new medItkCloseProcessPrivate)
{
    this->setFilter(this);

    this->setDescription(tr("ITK close filter"));

    d->radius = new medDoubleParameter("Radius");
    d->radius->setRange(0,10);
    d->radius->setValue(1.0);

    this->accessParameters() << d->radius;
}

medItkCloseProcess::~medItkCloseProcess()
{
}

//-------------------------------------------------------------------------------------------

bool medItkCloseProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("medItkCloseProcess", createmedItkCloseProcess, "medAbstractFilteringProcess");
}

//-------------------------------------------------------------------------------------------

dtkAbstractProcess * createmedItkCloseProcess()
{
    medItkFiltersProcessBase* baseProcess = new medItkCloseProcessDerived;
    medItkWrapper* aWrapper = new TemplateWrapper<itk::GrayscaleMorphologicalClosingImageFilter>(baseProcess);
    baseProcess->setWrapper(aWrapper);
    return baseProcess;
}
