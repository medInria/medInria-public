#include "itkProcessTensorLogEuclideanAnisotropicSmoothing.h"

#include <dtkCore/dtkSmartPointer.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>
#include <medAbstractData.h>

#include "itkAnisotropicDiffusionTensorImageFilter.h"
#include "itkLogTensorImageFilter.h"
#include "itkExpTensorImageFilter.h"
#include "itkTensor.h"
#include "itkImage.h"

#include "itkQtSignalAdaptor.h"

// /////////////////////////////////////////////////////////////////
// itkProcessTensorLogEuclideanAnisotropicSmoothingPrivate
// /////////////////////////////////////////////////////////////////

class itkProcessTensorLogEuclideanAnisotropicSmoothingPrivate
{
public:

    typedef float                       ScalarType;
    typedef itk::Tensor<ScalarType, 3>  TensorType;
    typedef itk::Image<TensorType, 3>   TensorImageType;

    typedef itk::LogTensorImageFilter<TensorImageType, TensorImageType> LogFilterType;
    typedef itk::ExpTensorImageFilter<TensorImageType, TensorImageType> ExpFilterType;
    typedef itk::AnisotropicDiffusionTensorImageFilter<TensorImageType, TensorImageType> FilterType;

    itkProcessTensorLogEuclideanAnisotropicSmoothingPrivate (void);
    ~itkProcessTensorLogEuclideanAnisotropicSmoothingPrivate (void);

public:

    LogFilterType::Pointer loger;
    ExpFilterType::Pointer exper;
    FilterType::Pointer    smoother;

    dtkSmartPointer<medAbstractData> output;
};


itkProcessTensorLogEuclideanAnisotropicSmoothingPrivate::itkProcessTensorLogEuclideanAnisotropicSmoothingPrivate()
{
}

itkProcessTensorLogEuclideanAnisotropicSmoothingPrivate::~itkProcessTensorLogEuclideanAnisotropicSmoothingPrivate()
{}

// /////////////////////////////////////////////////////////////////
// itkProcessTensorLogEuclideanAnisotropicSmoothing
// /////////////////////////////////////////////////////////////////

itkProcessTensorLogEuclideanAnisotropicSmoothing::itkProcessTensorLogEuclideanAnisotropicSmoothing(void) : medAbstractDiffusionProcess(), d (new itkProcessTensorLogEuclideanAnisotropicSmoothingPrivate)
{
    d->smoother = itkProcessTensorLogEuclideanAnisotropicSmoothingPrivate::FilterType::New();
    d->loger    = itkProcessTensorLogEuclideanAnisotropicSmoothingPrivate::LogFilterType::New();
    d->exper    = itkProcessTensorLogEuclideanAnisotropicSmoothingPrivate::ExpFilterType::New();

    d->output   = medAbstractDataFactory::instance()->createSmartPointer ("itkDataTensorImageFloat3");
    d->output->setData ( d->exper->GetOutput() );

    d->smoother->SetNumberOfIterations(250);
    d->smoother->SetKappa(0.05);
    d->smoother->SetLambda(0.2);
    d->smoother->SetTimeStep(0.1);
    d->smoother->SetRMSChange (0.0);

    d->smoother->SetInput (d->loger->GetOutput());
    d->exper->SetInput (d->smoother->GetOutput());

    itk::QtSignalAdaptor *adaptor = new itk::QtSignalAdaptor;
    d->smoother->AddObserver (itk::ProgressEvent(), adaptor->GetCommand());
    connect (adaptor, SIGNAL(Signal (int)), this, SIGNAL (progressed (int)));

    this->addProperty ("SmoothingLevel", QStringList() << "low" << "medium" << "high");
    this->setProperty("SmoothingLevel", "low");
}

itkProcessTensorLogEuclideanAnisotropicSmoothing::~itkProcessTensorLogEuclideanAnisotropicSmoothing(void)
{
    delete d;
    d = 0;
}

bool itkProcessTensorLogEuclideanAnisotropicSmoothing::registered(void)
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkProcessTensorLogEuclideanAnisotropicSmoothing", createItkProcessTensorLogEuclideanAnisotropicSmoothing);
}

QString itkProcessTensorLogEuclideanAnisotropicSmoothing::identifier(void) const
{
    return "itkProcessTensorLogEuclideanAnisotropicSmoothing";
}

QString itkProcessTensorLogEuclideanAnisotropicSmoothing::description(void) const
{
    return "itkProcessTensorLogEuclideanAnisotropicSmoothing";
}

void itkProcessTensorLogEuclideanAnisotropicSmoothing::setInputImage (medAbstractData* data)
{
    if (!data)
        return;

    typedef itkProcessTensorLogEuclideanAnisotropicSmoothingPrivate::TensorImageType TensorImageType;

    if (data->identifier()=="itkDataTensorImageFloat3") {
        if( TensorImageType* tensors = static_cast<TensorImageType*>(data->data()) )
            d->loger->SetInput ( tensors );
    }
    else {
        qDebug() << "Cannot cast input into a compatible tensor image";
        return;
    }
}

medAbstractData *itkProcessTensorLogEuclideanAnisotropicSmoothing::output (void)
{
    return d->output;
}

int itkProcessTensorLogEuclideanAnisotropicSmoothing::update (void)
{
    qDebug()<<"Start";

    try {
        d->exper->UpdateLargestPossibleRegion();
    }
    catch ( itk::ExceptionObject &e) {
        qDebug() << "Abort";
        d->loger->AbortGenerateDataOff();
        d->loger->ResetPipeline();
        d->loger->GetOutput()->Initialize();

        d->smoother->AbortGenerateDataOff();
        d->smoother->ResetPipeline();
        d->smoother->SetStateToUninitialized();
        d->smoother->GetOutput()->Initialize();

        d->exper->AbortGenerateDataOff();
        d->exper->ResetPipeline();
        d->exper->GetOutput()->Initialize();

	qDebug() << e.GetDescription();
	return -1;
    }

    return 0;
}

void itkProcessTensorLogEuclideanAnisotropicSmoothing::onPropertySet (const QString &key, const QString &value)
{
    if (key=="SmoothingLevel")
        this->onSmoothingLevelPropertySet (value);
}

void itkProcessTensorLogEuclideanAnisotropicSmoothing::onSmoothingLevelPropertySet(const QString &value)
{
    qDebug() << "Smoothing set to: " << value;

    if (value=="low") {
        d->smoother->SetNumberOfIterations(250);
        d->smoother->SetKappa(0.05);
        d->smoother->SetLambda(0.2);
        d->smoother->SetTimeStep(0.1);
        d->smoother->SetRMSChange (0.0);
    }

    if (value=="medium") {
        d->smoother->SetNumberOfIterations(250);
        d->smoother->SetKappa(0.1);
        d->smoother->SetLambda(0.2);
        d->smoother->SetTimeStep(0.1);
        d->smoother->SetRMSChange (0.0);
    }

    if (value=="high") {
        d->smoother->SetNumberOfIterations(250);
        d->smoother->SetKappa(0.2);
        d->smoother->SetLambda(0.2);
        d->smoother->SetTimeStep(0.1);
        d->smoother->SetRMSChange (0.0);
    }
}

void itkProcessTensorLogEuclideanAnisotropicSmoothing::onCanceled(void)
{
    d->loger->AbortGenerateDataOn();
    d->smoother->AbortGenerateDataOn();
    d->exper->AbortGenerateDataOn();
}

// /////////////////////////////////////////////////////////////////
// Process type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createItkProcessTensorLogEuclideanAnisotropicSmoothing(void)
{
    return new itkProcessTensorLogEuclideanAnisotropicSmoothing;
}

