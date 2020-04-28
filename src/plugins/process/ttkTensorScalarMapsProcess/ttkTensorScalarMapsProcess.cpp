#include "ttkTensorScalarMapsProcess.h"

#include <dtkLog>

#include <itkImage.h>
#include <itkTensor.h>
#include <itkRGBAPixel.h>

#include <medAbstractImageData.h>
#include <medAbstractDiffusionModelImageData.h>
#include <medAbstractDataFactory.h>

// ////////////////////////////////////////////
// Headers from TTK
#include <itkTensorToScalarTensorImageFilter.h>
#include <itkTensorToScalarFunction.h>
#include <itkTensorToFAFunction.h>
#include <itkTensorToColorFAFunction.h>
#include <itkTensorToLogFAFunction.h>
#include <itkTensorToADCFunction.h>
#include <itkTensorToClFunction.h>
#include <itkTensorToCpFunction.h>
#include <itkTensorToCsFunction.h>
#include <itkTensorToRAFunction.h>
#include <itkTensorToVRFunction.h>
#include <itkTensorToVolumeFunction.h>
#include <itkTensorToLambdaFunction.h>

#include <itkCommand.h>
#include <medMetaDataKeys.h>

#include <vtkSmartPointer.h>

ttkTensorScalarMapsProcess::ttkTensorScalarMapsProcess(QObject *parent)
    : medAbstractDiffusionScalarMapsProcess(parent)
{
    m_filter = 0;
    m_scalarMapRequested = "fa";
}

ttkTensorScalarMapsProcess::~ttkTensorScalarMapsProcess()
{
}

QString ttkTensorScalarMapsProcess::caption() const
{
    return "Tensor scalar maps";
}

QString ttkTensorScalarMapsProcess::description() const
{
    return "Compute scalar maps from tensors data";
}

void ttkTensorScalarMapsProcess::selectRequestedScalarMap(QString mapRequested)
{
    m_scalarMapRequested = mapRequested;
}

medAbstractJob::medJobExitStatus ttkTensorScalarMapsProcess::run()
{
    medAbstractJob::medJobExitStatus jobExitSatus = medAbstractJob::MED_JOB_EXIT_FAILURE;

    if(this->input())
    {
        QString id =  this->input()->identifier();

        if ( id == "itkDataTensorImageFloat3" )
        {
            jobExitSatus = this->_run<float>();
        }
        else if ( id == "itkDataTensorImageDouble3" )
        {
            jobExitSatus = this->_run<double>();
        }
    }

    return jobExitSatus;
}

template <class inputType>
medAbstractJob::medJobExitStatus ttkTensorScalarMapsProcess::_run()
{
    typedef itk::Tensor<inputType, 3> TensorType;
    typedef itk::Image<TensorType, 3> TensorImageType;
    typedef itk::Image<inputType, 3> ImageType;
    typedef itk::TensorToScalarTensorImageFilter<TensorImageType, ImageType> FilterType;

    typename TensorImageType::Pointer inData = dynamic_cast<TensorImageType *>((itk::Object*)(this->input()->data()));
    if (!inData)
        return medAbstractJob::MED_JOB_EXIT_FAILURE;

    itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
    callback->SetClientData((void*)this);
    callback->SetCallback(ttkTensorScalarMapsProcess::eventCallback);

    if (m_scalarMapRequested == "ColorFA")
    {
        typedef itk::RGBAPixel<unsigned char> ColorType;
        typedef itk::Image<ColorType, 3> ColorImageType;
        typedef itk::TensorToScalarTensorImageFilter<TensorImageType, ColorImageType> ColorFilterType;

        typename ColorFilterType::Pointer mainFilter = ColorFilterType::New();

        medAbstractImageData *output = qobject_cast <medAbstractImageData *> (medAbstractDataFactory::instance()->create ("itkDataImageRGBA3"));
        output->setData(mainFilter->GetOutput());

        m_filter = mainFilter;
        mainFilter->SetInput(inData);
        typedef itk::TensorToColorFAFunction<TensorType, ColorType> FunctionType;
        mainFilter->SetTensorToScalarFunction (FunctionType::New());
        FunctionType *colorFunction = dynamic_cast <FunctionType *> (mainFilter->GetTensorToScalarFunction().GetPointer());
        colorFunction->SetTransformColorWithDirection(true);
        colorFunction->SetDirection(mainFilter->GetInput()->GetDirection());
        mainFilter->AddObserver(itk::ProgressEvent(), callback);

        try
        {
            mainFilter->Update();
        }
        catch (itk::ProcessAborted &e)
        {
            return medAbstractJob::MED_JOB_EXIT_CANCELLED;
        }

        output->setMetaData(medMetaDataKeys::SeriesDescription.key(), this->input()->metadata(medMetaDataKeys::SeriesDescription.key()) + " " + m_scalarMapRequested);
        this->setOutput(output);
        return medAbstractJob::MED_JOB_EXIT_SUCCESS;
    }

    typename FilterType::Pointer filter = FilterType::New();
    filter->SetInput(inData);
    filter->AddObserver(itk::ProgressEvent(), callback);
    m_filter = filter;

    if( m_scalarMapRequested=="FA" )
        filter->SetTensorToScalarFunction( itk::TensorToFAFunction<TensorType, inputType>::New() );
    else if( m_scalarMapRequested=="LogFA" )
        filter->SetTensorToScalarFunction( itk::TensorToLogFAFunction<TensorType, inputType>::New() );
    else if( m_scalarMapRequested=="ADC" )
        filter->SetTensorToScalarFunction( itk::TensorToADCFunction<TensorType, inputType>::New() );
    else if( m_scalarMapRequested=="Cl" )
        filter->SetTensorToScalarFunction( itk::TensorToClFunction<TensorType, inputType>::New() );
    else if( m_scalarMapRequested=="Cp" )
        filter->SetTensorToScalarFunction( itk::TensorToCpFunction<TensorType, inputType>::New() );
    else if( m_scalarMapRequested=="Cs" )
        filter->SetTensorToScalarFunction( itk::TensorToCsFunction<TensorType, inputType>::New() );
    else if( m_scalarMapRequested=="RA" )
        filter->SetTensorToScalarFunction( itk::TensorToRAFunction<TensorType, inputType>::New() );
    else if( m_scalarMapRequested=="VR" )
        filter->SetTensorToScalarFunction( itk::TensorToVRFunction<TensorType, inputType>::New() );
    else if( m_scalarMapRequested=="Lambda1" )
    {
        typename itk::TensorToLambdaFunction<TensorType, inputType>::Pointer function = itk::TensorToLambdaFunction<TensorType, inputType>::New();
        function->SetLambdaIndex (2);
        filter->SetTensorToScalarFunction( function );
    }
    else if( m_scalarMapRequested=="Lambda2" )
    {
        typename itk::TensorToLambdaFunction<TensorType, inputType>::Pointer function = itk::TensorToLambdaFunction<TensorType, inputType>::New();
        function->SetLambdaIndex (1);
        filter->SetTensorToScalarFunction( function );
    }
    else if( m_scalarMapRequested=="Lambda3" )
    {
        typename itk::TensorToLambdaFunction<TensorType, inputType>::Pointer function = itk::TensorToLambdaFunction<TensorType, inputType>::New();
        function->SetLambdaIndex (0);
        filter->SetTensorToScalarFunction( function );
    }

    try
    {
        filter->Update();
    }
    catch(itk::ProcessAborted &e)
    {
        return medAbstractJob::MED_JOB_EXIT_CANCELLED;
    }

    medAbstractImageData *output;
    if (this->input()->identifier().contains("Double"))
        output = qobject_cast <medAbstractImageData *> (medAbstractDataFactory::instance()->create ("itkDataImageDouble3"));
    else
        output = qobject_cast <medAbstractImageData *> (medAbstractDataFactory::instance()->create ("itkDataImageFloat3"));

    output->setData(filter->GetOutput());
    output->setMetaData(medMetaDataKeys::SeriesDescription.key(), this->input()->metadata(medMetaDataKeys::SeriesDescription.key()) + " " + m_scalarMapRequested);

    this->setOutput(output);

    return medAbstractJob::MED_JOB_EXIT_SUCCESS;
}

void ttkTensorScalarMapsProcess::cancel()
{
    if(this->isRunning())
    {
        if (m_filter.IsNotNull())
            m_filter->AbortGenerateDataOn();
    }
}
