#include <medItkErodeProcess.h>

#include <medItkWrapper.h>

#include <itkGrayscaleErodeImageFilter.h>
#include <itkBinaryBallStructuringElement.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkImage.h>
#include <itkCommand.h>
#include <itkExceptionObject.h>

#include <medMetaDataKeys.h>
#include <medAbstractDataFactory.h>

#include <medDoubleParameter.h>

struct medItkErodeProcessPrivate
{
    medDoubleParameter* radius;
};

class medItkErodeProcessDerived : public medItkErodeProcess
{
public:
    medItkErodeProcessDerived(medItkFiltersProcessBase* parent = NULL) : medItkErodeProcess(parent) { }
    medItkErodeProcessPrivate* internal()
    {
        return d;
    }
};

template <>
class PrototypeInternal3<itk::GrayscaleErodeImageFilter>
{
    public:
    template <class T>
    int update(medItkFiltersProcessBase* base)
    {
        typedef itk::Image< T, 3 > ImageType;
        typedef itk::BinaryBallStructuringElement < T, 3> StructuringElementType;
        typedef itk::GrayscaleErodeImageFilter< ImageType, ImageType, StructuringElementType > ErodeType;
        typename ErodeType::Pointer erodeFilter = ErodeType::New();

        medItkErodeProcessDerived* process = dynamic_cast<medItkErodeProcessDerived*>(base);

        if (!process)
            return EXIT_FAILURE;

        StructuringElementType ball;
        ball.SetRadius(process->internal()->radius->value());
        ball.CreateStructuringElement();

        erodeFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( process->input<medAbstractData>(0)->data() ) ) );

        erodeFilter->SetKernel ( ball );
        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) process );
        process->setCallBack(callback);

        erodeFilter->AddObserver ( itk::ProgressEvent(), callback );
        erodeFilter->Update();

        process->output<medAbstractData>(0)->setData ( erodeFilter->GetOutput() );

        QString newSeriesDescription = process->input<medAbstractData>(0)->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " Erode filter (" + QString::number(process->internal()->radius->value()) + ")";
        process->output<medAbstractData>(0)->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );

        return EXIT_SUCCESS;
    }

    QString resultString;
};





medItkErodeProcess::medItkErodeProcess(medItkFiltersProcessBase *parent):
    medItkFiltersProcessBase(parent), d(new medItkErodeProcessPrivate)
{
    this->setFilter(this);

    this->setDescription(tr("ITK Erode filter"));

    d->radius = new medDoubleParameter("Radius");
    d->radius->setRange(0,10);
    d->radius->setValue(1.0);

    this->accessParameters() << d->radius;
}

medItkErodeProcess::~medItkErodeProcess()
{
}

//-------------------------------------------------------------------------------------------

bool medItkErodeProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("medItkErodeProcess", createmedItkErodeProcess, "medAbstractFilteringProcess");
}

//-------------------------------------------------------------------------------------------

dtkAbstractProcess * createmedItkErodeProcess()
{
    medItkFiltersProcessBase* baseProcess = new medItkErodeProcessDerived;
    medItkWrapper* aWrapper = new TemplateWrapper3<itk::GrayscaleErodeImageFilter>(baseProcess);
    baseProcess->setWrapper(aWrapper);
    return baseProcess;
}
