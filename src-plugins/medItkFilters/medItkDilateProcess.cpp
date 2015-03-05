#include <medItkDilateProcess.h>

#include <medItkWrapper.h>

#include <itkGrayscaleDilateImageFilter.h>
#include <itkBinaryBallStructuringElement.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkImage.h>
#include <itkCommand.h>
#include <itkExceptionObject.h>

#include <medMetaDataKeys.h>
#include <medAbstractDataFactory.h>

#include <medDoubleParameter.h>

struct medItkDilateProcessPrivate
{
    medDoubleParameter* radius;
};

class medItkDilateProcessDerived : public medItkDilateProcess
{
public:
    medItkDilateProcessDerived(medItkFiltersProcessBase* parent = NULL) : medItkDilateProcess(parent) { }
    medItkDilateProcessPrivate* internal()
    {
        return d;
    }
};

template <>
class PrototypeInternal<itk::GrayscaleDilateImageFilter>
{
    public:
    template <class T>
    int update(medItkFiltersProcessBase* base)
    {
        typedef itk::Image< T, 3 > ImageType;
        typedef itk::BinaryBallStructuringElement < T, 3> StructuringElementType;
        typedef itk::GrayscaleDilateImageFilter< ImageType, ImageType, StructuringElementType > DilateType;
        typename DilateType::Pointer dilateType = DilateType::New();

        medItkDilateProcessDerived* process = dynamic_cast<medItkDilateProcessDerived*>(base);

        if (!process)
            return EXIT_FAILURE;

        StructuringElementType ball;
        ball.SetRadius(process->internal()->radius->value());
        ball.CreateStructuringElement();

        dilateType->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( process->input<medAbstractData>(0)->data() ) ) );

        dilateType->SetKernel ( ball );
        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) process );
        process->setCallBack(callback);

        dilateType->AddObserver ( itk::ProgressEvent(), callback );
        dilateType->Update();

        process->output<medAbstractData>(0)->setData ( dilateType->GetOutput() );

        QString newSeriesDescription = process->input<medAbstractData>(0)->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " Close filter (" + QString::number(process->internal()->radius->value()) + ")";
        process->output<medAbstractData>(0)->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );

        return EXIT_SUCCESS;
    }

    QString resultString;
};





medItkDilateProcess::medItkDilateProcess(medItkFiltersProcessBase *parent):
    medItkFiltersProcessBase(parent), d(new medItkDilateProcessPrivate)
{
    this->setFilter(this);

    this->setDescription(tr("ITK dilate filter"));

    d->radius = new medDoubleParameter("Radius");
    d->radius->setRange(0,10);
    d->radius->setValue(1.0);

    this->accessParameters() << d->radius;

}

medItkDilateProcess::~medItkDilateProcess()
{
}

//-------------------------------------------------------------------------------------------

bool medItkDilateProcess::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("medItkDilateProcess", createmedItkDilateProcess, "medAbstractFilteringProcess");
}

//-------------------------------------------------------------------------------------------

dtkAbstractProcess * createmedItkDilateProcess()
{
    medItkFiltersProcessBase* baseProcess = new medItkDilateProcessDerived;
    medItkWrapper* aWrapper = new TemplateWrapper<itk::GrayscaleDilateImageFilter>(baseProcess);
    baseProcess->setWrapper(aWrapper);
    return baseProcess;
}
