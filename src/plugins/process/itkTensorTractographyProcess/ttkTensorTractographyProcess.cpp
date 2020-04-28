#include "ttkTensorTractographyProcess.h"

#include <dtkLog>

#include <itkImage.h>
#include <itkTensor.h>

#include <medAbstractImageData.h>
#include <medAbstractFibersData.h>
#include <medAbstractDiffusionModelImageData.h>
#include <medAbstractDataFactory.h>

#include <itkFiber.h>
#include <itkFiberTrackingImageFilter.h>
#include <itkLogTensorImageFilter.h>
#include <itkFiberImageToVtkPolyData.h>
#include <itkCommand.h>

#include <vtkSmartPointer.h>

ttkTensorTractographyProcess::ttkTensorTractographyProcess(QObject *parent)
    : medAbstractTractographyProcess(parent)
{
    m_logerfilter = 0;
    m_tractographyfilter = 0;
    m_converterfilter = 0;

    m_faThreshold = new medIntParameter("fa_threshold", this);
    m_faThreshold->setCaption("Starting FA threshold");
    m_faThreshold->setDescription("Start seeding fibers in voxels whose FA value is above the threshold.");
    m_faThreshold->setRange(0,1000);
    m_faThreshold->setValue(300);

    m_faThreshold2 = new medIntParameter("fa_threshold2", this);
    m_faThreshold2->setCaption("Stopping FA threshold");
    m_faThreshold2->setDescription("Stop fiber's construction when reaching a voxel\nwhose FA value falls below the threshold.");
    m_faThreshold2->setRange(0,1000);
    m_faThreshold2->setValue(200);

    m_smoothness = new medIntParameter("smoothness", this);
    m_smoothness->setCaption("Smoothness");
    m_smoothness->setDescription("This value indicates how \"strictly\" the fiber follows the tensors.\nThe higher it is, the less the fiber gets deviated by the tensors.");
    m_smoothness->setRange(0,100);
    m_smoothness->setValue(0);

    m_minLength = new medIntParameter("min_length", this);
    m_minLength->setCaption("Minimum length");
    m_minLength->setDescription("At the final stage, fibers whose length is below the chosen amount (in millimeters) are deleted.");
    m_minLength->setRange(0,100);
    m_minLength->setValue(10);

    m_sampling = new medIntParameter("sampling", this);
    m_sampling->setCaption("Sampling");
    m_sampling->setDescription("Fibers are seeded every X voxels, being X the sampling value.");
    m_sampling->setRange(1,100);
    m_sampling->setValue(1);
}

ttkTensorTractographyProcess::~ttkTensorTractographyProcess()
{
}

QString ttkTensorTractographyProcess::caption() const
{
    return "TTK tensor tractography";
}

QString ttkTensorTractographyProcess::description() const
{
    return "Use TTK to track fibers from tensors data";
}

medIntParameter *ttkTensorTractographyProcess::faThreshold() const
{
    return m_faThreshold;
}

medIntParameter *ttkTensorTractographyProcess::faThreshold2() const
{
    return m_faThreshold2;
}

medIntParameter *ttkTensorTractographyProcess::smoothness() const
{
    return m_smoothness;
}

medIntParameter *ttkTensorTractographyProcess::minLength() const
{
    return m_minLength;
}

medIntParameter *ttkTensorTractographyProcess::sampling() const
{
    return m_sampling;
}

medAbstractJob::medJobExitStatus ttkTensorTractographyProcess::run()
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
medAbstractJob::medJobExitStatus ttkTensorTractographyProcess::_run()
{
    typedef itk::Tensor<inputType, 3> TensorType;
    typedef itk::Image<TensorType, 3> TensorImageType;

    typename TensorImageType::Pointer inData = dynamic_cast<TensorImageType *>((itk::Object*)(this->input()->data()));
    if (!inData)
        return medAbstractJob::MED_JOB_EXIT_FAILURE;

    typedef itk::LogTensorImageFilter<TensorImageType, TensorImageType> LogFilterType;
    typedef itk::Fiber<inputType, 3> FiberType;
    typedef itk::Image<FiberType, 3> FiberImageType;
    typedef itk::FiberTrackingImageFilter<TensorImageType, FiberImageType> FiberTrackingFilterType;
    typedef itk::FiberImageToVtkPolyData<FiberImageType> FiberImageToVtkPolyDataType;

    typename LogFilterType::Pointer logFilter = LogFilterType::New();
    logFilter->SetInput(inData);
    m_logerfilter = logFilter;

    try
    {
        logFilter->Update();
    }
    catch(itk::ProcessAborted &e)
    {
        return medAbstractJob::MED_JOB_EXIT_CANCELLED;
    }

    typename FiberTrackingFilterType::Pointer trackerFilter = FiberTrackingFilterType::New();
    trackerFilter->SetInput(inData);
    trackerFilter->SetLogTensorImage (logFilter->GetOutput());

    trackerFilter->SetIntegrationMethod (2);
    trackerFilter->SetUseTriLinearInterpolation (1);
    trackerFilter->SetTimeStep (0.5);
    trackerFilter->SetOutputFiberSampling (1.0);
    trackerFilter->SetFAThreshold (m_faThreshold->value() / 1000.0);
    trackerFilter->SetFAThreshold2 (m_faThreshold2->value() / 1000.0);
    trackerFilter->SetSmoothness (m_smoothness->value());
    trackerFilter->SetSampling (m_sampling->value());
    trackerFilter->SetTransformTensorWithImageDirection (1);
    trackerFilter->SetMinLength (m_minLength->value());
    trackerFilter->SetMaxLength (200.0);
    m_tractographyfilter = trackerFilter;

    itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
    callback->SetClientData((void*)this);
    callback->SetCallback(ttkTensorTractographyProcess::eventCallback);
    trackerFilter->AddObserver(itk::ProgressEvent(), callback);

    try
    {
        trackerFilter->Update();
    }
    catch(itk::ProcessAborted &e)
    {
        return medAbstractJob::MED_JOB_EXIT_CANCELLED;
    }

    typename FiberImageToVtkPolyDataType::Pointer converterFilter = FiberImageToVtkPolyDataType::New();
    converterFilter->SetInput(trackerFilter->GetOutput());
    m_converterfilter = converterFilter;

    try
    {
        converterFilter->Update();
    }
    catch(itk::ProcessAborted &e)
    {
        return medAbstractJob::MED_JOB_EXIT_CANCELLED;
    }

    medAbstractFibersData *output = dynamic_cast <medAbstractFibersData *> (medAbstractDataFactory::instance()->create("medVtkFibersData"));

    if (output)
        output->setData (converterFilter->GetOutput());

    this->setOutput(output);

    return medAbstractJob::MED_JOB_EXIT_SUCCESS;
}

void ttkTensorTractographyProcess::cancel()
{
    if(this->isRunning())
    {
        if (m_tractographyfilter.IsNotNull())
            m_tractographyfilter->AbortGenerateDataOn();

        if (m_logerfilter.IsNotNull())
            m_logerfilter->AbortGenerateDataOn();

        if (m_converterfilter.IsNotNull())
            m_converterfilter->AbortGenerateDataOn();
    }
}
