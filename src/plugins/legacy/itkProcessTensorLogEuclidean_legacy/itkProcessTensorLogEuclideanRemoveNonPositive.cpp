#include "itkProcessTensorLogEuclideanRemoveNonPositive.h"

#include <dtkCore/dtkSmartPointer.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <medAbstractDataFactory.h>
#include <medAbstractData.h>

#include "itkRemoveNonPositiveTensorsTensorImageFilter.h"
#include "itkTensor.h"
#include "itkImage.h"

#include "itkQtSignalAdaptor.h"

// /////////////////////////////////////////////////////////////////
// itkProcessTensorLogEuclideanRemoveNonPositivePrivate
// /////////////////////////////////////////////////////////////////

class itkProcessTensorLogEuclideanRemoveNonPositivePrivate
{
public:

    typedef float                       ScalarType;
    typedef itk::Tensor<ScalarType, 3>  TensorType;
    typedef itk::Image<TensorType, 3>   TensorImageType;

    typedef itk::RemoveNonPositiveTensorsTensorImageFilter<TensorImageType, TensorImageType> FilterType;

    itkProcessTensorLogEuclideanRemoveNonPositivePrivate (void);
    ~itkProcessTensorLogEuclideanRemoveNonPositivePrivate (void);

public:

    FilterType::Pointer nptRemover;
    dtkSmartPointer<medAbstractData> output;
};


itkProcessTensorLogEuclideanRemoveNonPositivePrivate::itkProcessTensorLogEuclideanRemoveNonPositivePrivate()
{
}

itkProcessTensorLogEuclideanRemoveNonPositivePrivate::~itkProcessTensorLogEuclideanRemoveNonPositivePrivate()
{}

// /////////////////////////////////////////////////////////////////
// itkProcessTensorLogEuclideanRemoveNonPositive
// /////////////////////////////////////////////////////////////////

itkProcessTensorLogEuclideanRemoveNonPositive::itkProcessTensorLogEuclideanRemoveNonPositive(void) : medAbstractDiffusionProcess(), d (new itkProcessTensorLogEuclideanRemoveNonPositivePrivate)
{
    d->nptRemover = itkProcessTensorLogEuclideanRemoveNonPositivePrivate::FilterType::New();
    d->output     = medAbstractDataFactory::instance()->createSmartPointer ("itkDataTensorImageFloat3");
    d->output->setData ( d->nptRemover->GetOutput() );

    d->nptRemover->SetRemovingType (1);

    itk::QtSignalAdaptor *adaptor = new itk::QtSignalAdaptor;
    d->nptRemover->AddObserver (itk::ProgressEvent(), adaptor->GetCommand());
    connect (adaptor, SIGNAL(Signal (int)), this, SIGNAL (progressed (int)));

    this->addProperty ("RemovalType", QStringList() << "null" << "average");
}

itkProcessTensorLogEuclideanRemoveNonPositive::~itkProcessTensorLogEuclideanRemoveNonPositive(void)
{
    delete d;
    d = 0;
}

bool itkProcessTensorLogEuclideanRemoveNonPositive::registered(void)
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkProcessTensorLogEuclideanRemoveNonPositive", createItkProcessTensorLogEuclideanRemoveNonPositive);
}

QString itkProcessTensorLogEuclideanRemoveNonPositive::identifier(void) const
{
    return "itkProcessTensorLogEuclideanRemoveNonPositive";
}

QString itkProcessTensorLogEuclideanRemoveNonPositive::description(void) const
{
    return "itkProcessTensorLogEuclideanRemoveNonPositive";
}

void itkProcessTensorLogEuclideanRemoveNonPositive::setInputImage (medAbstractData* data)
{
    if (!data)
        return;

    typedef itkProcessTensorLogEuclideanRemoveNonPositivePrivate::TensorImageType TensorImageType;

    if (data->identifier()=="itkDataTensorImageFloat3") {
        if( TensorImageType* tensors = static_cast<TensorImageType*>(data->data()) )
	    d->nptRemover->SetInput ( tensors );
    }
    else {
        qDebug() << "Cannot cast input into a compatible tensor image";
        return;
    }
}

medAbstractData *itkProcessTensorLogEuclideanRemoveNonPositive::output (void)
{
    return d->output;
}

int itkProcessTensorLogEuclideanRemoveNonPositive::update (void)
{
    try {
        d->nptRemover->UpdateLargestPossibleRegion();
    }
    catch ( itk::ExceptionObject &e) {
        d->nptRemover->AbortGenerateDataOff();
	d->nptRemover->ResetPipeline();
	d->nptRemover->GetOutput()->Initialize();

	qDebug() << e.GetDescription();
	return -1;
    }

    return 0;
}

void itkProcessTensorLogEuclideanRemoveNonPositive::onPropertySet (const QString &key, const QString &value)
{
    if (key=="RemovalType")
        this->onRemovalTypePropertySet (value);
}

void itkProcessTensorLogEuclideanRemoveNonPositive::onRemovalTypePropertySet (const QString &value)
{
    if (value=="null")
        d->nptRemover->SetRemovingType (0);

    if (value=="average")
        d->nptRemover->SetRemovingType (1);

}

// /////////////////////////////////////////////////////////////////
// Process type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess *createItkProcessTensorLogEuclideanRemoveNonPositive(void)
{
    return new itkProcessTensorLogEuclideanRemoveNonPositive;
}

