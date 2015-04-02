#include <medItkOpenProcess.h>

#include <medItkWrapper.h>

#include <itkGrayscaleMorphologicalOpeningImageFilter.h>
#include <itkBinaryBallStructuringElement.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkImage.h>
#include <itkCommand.h>
#include <itkExceptionObject.h>

#include <medMetaDataKeys.h>
#include <medAbstractDataFactory.h>

#include <medDoubleParameter.h>

struct medItkOpenProcessPrivate
{
    medDoubleParameter* radius;
};

class medItkOpenProcessDerived : public medItkOpenProcess
{
public:
    medItkOpenProcessDerived(medItkFiltersProcessBase* parent = NULL) : medItkOpenProcess(parent) { }
    medItkOpenProcessPrivate* internal()
    {
        return d;
    }
};

template <>
class PrototypeInternal3<itk::GrayscaleMorphologicalOpeningImageFilter>
{
    public:
    template <class T>
    int update(medItkFiltersProcessBase* base)
    {
        typedef itk::Image< T, 3 > ImageType;
        typedef itk::BinaryBallStructuringElement < T, 3> StructuringElementType;
        typedef itk::GrayscaleMorphologicalOpeningImageFilter< ImageType, ImageType, StructuringElementType > OpenType;
        typename OpenType::Pointer openFilter = OpenType::New();

        medItkOpenProcessDerived* process = dynamic_cast<medItkOpenProcessDerived*>(base);

        if (!process)
            return EXIT_FAILURE;

        StructuringElementType ball;
        ball.SetRadius(process->internal()->radius->value());
        ball.CreateStructuringElement();

        openFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( process->input<medAbstractData>(0)->data() ) ) );

        openFilter->SetKernel ( ball );
        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) process );
        process->setCallBack(callback);

        openFilter->AddObserver ( itk::ProgressEvent(), callback );
        openFilter->Update();

        process->output<medAbstractData>(0)->setData ( openFilter->GetOutput() );

        QString newSeriesDescription = process->input<medAbstractData>(0)->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " Open filter (" + QString::number(process->internal()->radius->value()) + ")";
        process->output<medAbstractData>(0)->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );

        return EXIT_SUCCESS;
    }

    QString resultString;
};





medItkOpenProcess::medItkOpenProcess(medItkFiltersProcessBase *parent):
    medItkFiltersProcessBase(parent), d(new medItkOpenProcessPrivate)
{
    this->setFilter(this);

    this->setDescription(tr("ITK Open filter"));

    d->radius = new medDoubleParameter("Radius");
    d->radius->setRange(0,10);
    d->radius->setValue(1.0);

    this->accessParameters() << d->radius;
}

medItkOpenProcess::~medItkOpenProcess()
{
}

//-------------------------------------------------------------------------------------------

bool medItkOpenProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("medItkOpenProcess", createmedItkOpenProcess, "medAbstractFilteringProcess");
}

//-------------------------------------------------------------------------------------------

dtkAbstractProcess * createmedItkOpenProcess()
{
    medItkFiltersProcessBase* baseProcess = new medItkOpenProcessDerived;
    medItkWrapper* aWrapper = new TemplateWrapper3<itk::GrayscaleMorphologicalOpeningImageFilter>(baseProcess);
    baseProcess->setWrapper(aWrapper);
    return baseProcess;
}
