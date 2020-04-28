#include "ttkTensorEstimationProcess.h"

#include <dtkLog>

#include <itkImage.h>
#include <itkTensor.h>
#include <itkDTIEstimatorTensorImageFilter.h>
#include <itkRemoveNonPositiveTensorsTensorImageFilter.h>
#include <itkAnisotropicDiffusionTensorImageFilter.h>
#include <itkLogTensorImageFilter.h>
#include <itkExpTensorImageFilter.h>
#include <itkExtractImageFilter.h>

#include <medAbstractImageData.h>
#include <medAbstractDiffusionModelImageData.h>
#include <medAbstractDataFactory.h>

ttkTensorEstimationProcess::ttkTensorEstimationProcess(QObject *parent)
    : medAbstractDiffusionModelEstimationProcess(parent)
{
    m_estimationfilter = 0;
    m_smoothingfilter = 0;

    m_smoothing = new medBoolParameterGroup(this);

    m_NoSmoothing = new medBoolParameter("No smoothing", this);
    m_NoSmoothing->setCaption("None");
    m_NoSmoothing->setDescription("No smoothing applied");
    m_NoSmoothing->setValue(true);
    m_smoothing->addBoolParameter(m_NoSmoothing);

    m_LowSmoothing = new medBoolParameter("Low smoothing", this);
    m_LowSmoothing->setCaption("Low");
    m_LowSmoothing->setDescription("Low anisotropic smoothing");
    m_LowSmoothing->setValue(false);
    m_smoothing->addBoolParameter(m_LowSmoothing);

    m_MediumSmoothing = new medBoolParameter("Medium smoothing", this);
    m_MediumSmoothing->setCaption("Medium");
    m_MediumSmoothing->setDescription("Medium anisotropic smoothing");
    m_MediumSmoothing->setValue(false);
    m_smoothing->addBoolParameter(m_MediumSmoothing);

    m_HighSmoothing = new medBoolParameter("High smoothing", this);
    m_HighSmoothing->setCaption("High");
    m_HighSmoothing->setDescription("High anisotropic smoothing");
    m_HighSmoothing->setValue(false);
    m_smoothing->addBoolParameter(m_HighSmoothing);
}

ttkTensorEstimationProcess::~ttkTensorEstimationProcess()
{
}

QString ttkTensorEstimationProcess::caption() const
{
    return "TTK tensor estimation";
}

QString ttkTensorEstimationProcess::description() const
{
    return "Use TTK to estimate tensor models from DWI data";
}

medBoolParameter *ttkTensorEstimationProcess::noSmoothing() const
{
    return m_NoSmoothing;
}

medBoolParameter *ttkTensorEstimationProcess::lowSmoothing() const
{
    return m_LowSmoothing;
}

medBoolParameter *ttkTensorEstimationProcess::mediumSmoothing() const
{
    return m_MediumSmoothing;
}

medBoolParameter *ttkTensorEstimationProcess::highSmoothing() const
{
    return m_HighSmoothing;
}

medAbstractJob::medJobExitStatus ttkTensorEstimationProcess::run()
{
    medAbstractJob::medJobExitStatus jobExitSatus = medAbstractJob::MED_JOB_EXIT_FAILURE;

    if(this->input())
    {
        this->extractGradientsFromInformation();
        QString id =  this->input()->identifier();

        if ( id == "itkDataImageChar4" )
        {
            jobExitSatus = this->_run<char>();
        }
        else if ( id == "itkDataImageUChar4" )
        {
            jobExitSatus = this->_run<unsigned char>();
        }
        else if ( id == "itkDataImageShort4" )
        {
            jobExitSatus = this->_run<short>();
        }
        else if ( id == "itkDataImageUShort4" )
        {
            jobExitSatus = this->_run<unsigned short>();
        }
        else if ( id == "itkDataImageInt4" )
        {
            jobExitSatus = this->_run<int>();
        }
        else if ( id == "itkDataImageUInt4" )
        {
            jobExitSatus = this->_run<unsigned int>();
        }
        else if ( id == "itkDataImageLong4" )
        {
            jobExitSatus = this->_run<long>();
        }
        else if ( id== "itkDataImageULong4" )
        {
            jobExitSatus = this->_run<unsigned long>();
        }
        else if ( id == "itkDataImageFloat4" )
        {
            jobExitSatus = this->_run<float>();
        }
        else if ( id == "itkDataImageDouble4" )
        {
            jobExitSatus = this->_run<double>();
        }
    }

    return jobExitSatus;
}

template <class inputType>
medAbstractJob::medJobExitStatus ttkTensorEstimationProcess::_run()
{
    typedef itk::Image <inputType,4> DWIImageType;
    typedef itk::Image <inputType,3> SingleDWIImageType;
    typename DWIImageType::Pointer inData = dynamic_cast<DWIImageType *>((itk::Object*)(this->input()->data()));

    typedef float ScalarType;
    typedef itk::Tensor<ScalarType, 3> TensorType;
    typedef itk::Image<TensorType, 3> TensorImageType;

    if (!inData)
        return medAbstractJob::MED_JOB_EXIT_FAILURE;

    typedef itk::DTIEstimatorTensorImageFilter < SingleDWIImageType, TensorImageType > TensorEstimatorType;
    typedef typename TensorEstimatorType::GradientType GradientType;
    typedef typename TensorEstimatorType::GradientListType GradientListType;

    GradientsVectorType diffGrads = this->gradients();
    GradientListType gradientList(diffGrads.size());
    for (unsigned int i = 0;i < diffGrads.size();++i)
    {
        GradientType grad;
        for (unsigned int j = 0;j < 3;++j)
            grad[j] = diffGrads[i][j];

        gradientList[i] = grad;
    }

    typename TensorEstimatorType::Pointer filter = TensorEstimatorType::New();

    typedef itk::ExtractImageFilter<DWIImageType, SingleDWIImageType> ExtractorType;
    typename DWIImageType::SizeType size = inData->GetLargestPossibleRegion().GetSize();
    unsigned int imageCount = size[3];

    if (imageCount != diffGrads.size())
    {
        dtkWarn() << "Number of gradients not matching number of DWI images";
        return medAbstractJob::MED_JOB_EXIT_FAILURE;
    }

    size[3] = 0;
    typename DWIImageType::IndexType index = {{0,0,0,0}};
    typename DWIImageType::RegionType region;
    region.SetSize (size);
    for (unsigned int i=0; i<imageCount; i++)
    {
        index[3] = i;
        region.SetIndex (index);
        typename ExtractorType::Pointer extractor = ExtractorType::New();
        extractor->SetExtractionRegion (region);
        extractor->SetInput (inData);
        extractor->SetDirectionCollapseToGuess();
        try
        {
            extractor->Update();
        }
        catch (itk::ExceptionObject &e)
        {
            qDebug() << e.GetDescription();
            return medAbstractJob::MED_JOB_EXIT_FAILURE;
        }

       filter->SetInput(i, extractor->GetOutput());
    }

    filter->SetGradientList(gradientList);
    filter->SetBST(0);

    typedef itk::RemoveNonPositiveTensorsTensorImageFilter <TensorImageType,TensorImageType> NonPositiveRemoverType;
    typename NonPositiveRemoverType::Pointer nptRemoverFilter = NonPositiveRemoverType::New();

    nptRemoverFilter->SetRemovingType(1);
    nptRemoverFilter->SetInput(filter->GetOutput());
    m_estimationfilter = nptRemoverFilter;

    if (m_NoSmoothing->value())
    {
        try
        {
            m_estimationfilter->Update();
        }
        catch(itk::ExceptionObject &e)
        {
            qDebug() << e.GetDescription();
            return medAbstractJob::MED_JOB_EXIT_FAILURE;
        }

        medAbstractDiffusionModelImageData *out = qobject_cast<medAbstractDiffusionModelImageData *>(medAbstractDataFactory::instance()->create("itkDataTensorImageFloat3"));
        out->setData(nptRemoverFilter->GetOutput());
        this->setOutput(out);
        return medAbstractJob::MED_JOB_EXIT_SUCCESS;
    }

    typedef itk::LogTensorImageFilter<TensorImageType, TensorImageType> LogFilterType;
    typedef itk::ExpTensorImageFilter<TensorImageType, TensorImageType> ExpFilterType;
    typedef itk::AnisotropicDiffusionTensorImageFilter<TensorImageType, TensorImageType> AnisotropicFilterType;

    typename AnisotropicFilterType::Pointer smootherFilter = AnisotropicFilterType::New();

    if (m_LowSmoothing->value())
    {
        smootherFilter->SetNumberOfIterations(250);
        smootherFilter->SetKappa(0.05);
        smootherFilter->SetLambda(0.2);
        smootherFilter->SetTimeStep(0.1);
        smootherFilter->SetRMSChange(0.0);
    }
    else if (m_MediumSmoothing->value())
    {
        smootherFilter->SetNumberOfIterations(250);
        smootherFilter->SetKappa(0.1);
        smootherFilter->SetLambda(0.2);
        smootherFilter->SetTimeStep(0.1);
        smootherFilter->SetRMSChange(0.0);
    }
    else
    {
        smootherFilter->SetNumberOfIterations(250);
        smootherFilter->SetKappa(0.2);
        smootherFilter->SetLambda(0.2);
        smootherFilter->SetTimeStep(0.1);
        smootherFilter->SetRMSChange(0.0);
    }

    typename LogFilterType::Pointer logFilter = LogFilterType::New();
    logFilter->SetInput(nptRemoverFilter->GetOutput());

    smootherFilter->SetInput(logFilter->GetOutput());

    typename ExpFilterType::Pointer expFilter = ExpFilterType::New();
    expFilter->SetInput(smootherFilter->GetOutput());

    m_smoothingfilter = expFilter;

    try
    {
        m_smoothingfilter->Update();
    }
    catch(itk::ExceptionObject &e)
    {
        qDebug() << e.GetDescription();
        return medAbstractJob::MED_JOB_EXIT_FAILURE;
    }

    medAbstractDiffusionModelImageData *out = qobject_cast<medAbstractDiffusionModelImageData *>(medAbstractDataFactory::instance()->create("itkDataTensorImageFloat3"));
    out->setData(expFilter->GetOutput());
    this->setOutput(out);

    return medAbstractJob::MED_JOB_EXIT_SUCCESS;
}

void ttkTensorEstimationProcess::cancel()
{
    // Not implemented yet
}
