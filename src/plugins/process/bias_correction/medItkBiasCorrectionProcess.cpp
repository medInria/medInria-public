/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkBiasCorrectionProcess.h>

#include <dtkLog>

#include <itkImage.h>
#include <itkBinaryBallStructuringElement.h>
#include <itkCommand.h>

#include <medAbstractImageData.h>
#include <medAbstractDataFactory.h>

#include <medIntParameter.h>
#include <medDoubleParameter.h>
#include <medStringParameter.h>

#include <itkN4BiasFieldCorrectionImageFilter.h>
#include <itkExtractImageFilter.h>
#include <itkConstantPadImageFilter.h>
#include <itkOtsuThresholdImageFilter.h>
#include <itkShrinkImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkTimeProbe.h>

#define NB_STEPS 18

medItkBiasCorrectionProcess::medItkBiasCorrectionProcess(QObject *parent): medAbstractBiasCorrectionProcess(parent)
{
    m_bAborting = false;

    m_poUIThreadNb = new medIntParameter("ThreadNb", this);
    m_poUIThreadNb->setCaption("Number of threads");
    m_poUIThreadNb->setDescription("Set the number of computing treads");
    m_poUIThreadNb->setRange(1, itk::MultiThreaderBase::GetGlobalDefaultNumberOfThreads());
    m_poUIThreadNb->setValue(itk::MultiThreaderBase::GetGlobalDefaultNumberOfThreads());

    m_poUIShrinkFactors = new medIntParameter("Shrinkfactor", this);
    m_poUIShrinkFactors->setCaption("Shrink factor");
    m_poUIShrinkFactors->setDescription("Shrink factor");
    m_poUIShrinkFactors->setRange(1, 10);
    m_poUIShrinkFactors->setValue(4);

    m_poUISplineOrder = new medIntParameter("BSplineOrder", this);
    m_poUISplineOrder->setCaption("BSpline Order");
    m_poUISplineOrder->setDescription("BSpline Order");
    m_poUISplineOrder->setRange(1, 16);
    m_poUISplineOrder->setValue(3);

    m_poSMaxIterations = new medStringParameter("Iterations", this);
    m_poSMaxIterations->setCaption("Rounds of iterations");
    m_poSMaxIterations->setDescription("Number of round and number of iteration per round\n like 50x40x30");
    m_poSMaxIterations->setValidator(new QRegExpValidator(QRegExp("^[1-9][0-9]{0,2}(x[1-9][0-9]{0,2})*")));
    m_poSMaxIterations->setValue("50x40x30");

    m_poFWienerFilterNoise = new medDoubleParameter("WienerFilterNoise", this);
    m_poFWienerFilterNoise->setCaption("Wiener Filter Noise");
    m_poFWienerFilterNoise->setDescription("Wiener Filter Noise");
    m_poFWienerFilterNoise->setRange(0.000001, 1);
    m_poFWienerFilterNoise->setValue(0.01);

    m_poFbfFWHM = new medDoubleParameter("FbfFWHM", this);
    m_poFbfFWHM->setCaption("Bias field Full Width at Half Maximum");
    m_poFbfFWHM->setDescription("Bias field Full Width at Half Maximum");
    m_poFbfFWHM->setRange(0.0001, 1);
    m_poFbfFWHM->setValue(0.15);

    m_poFConvergenceThreshold = new medDoubleParameter("ConvergenceThreshold", this);
    m_poFConvergenceThreshold->setCaption("Convergence Threshold");
    m_poFConvergenceThreshold->setDescription("Convergence Threshold");
    m_poFConvergenceThreshold->setRange(0.00000001, 10);
    m_poFConvergenceThreshold->setValue(0.0001);

    m_poFSplineDistance = new medDoubleParameter("SplineDistance", this);
    m_poFSplineDistance->setCaption("B-Spline distance");
    m_poFSplineDistance->setDescription("B-Spline distance");
    m_poFSplineDistance->setRange(0, 128);
    m_poFSplineDistance->setValue(0);

    m_poFInitialMeshResolutionVect1 = new medDoubleParameter("XMeshResolution", this);
    m_poFInitialMeshResolutionVect1->setCaption("Spline Mesh resolution for X");
    m_poFInitialMeshResolutionVect1->setDescription("Mesh resolution for X");
    m_poFInitialMeshResolutionVect1->setRange(0.000001, 100);
    m_poFInitialMeshResolutionVect1->setValue(1);
    m_poFInitialMeshResolutionVect2 = new medDoubleParameter("YMeshResolution", this);
    m_poFInitialMeshResolutionVect2->setCaption("Spline Mesh resolution for Y");
    m_poFInitialMeshResolutionVect2->setDescription("Mesh resolution for Y");
    m_poFInitialMeshResolutionVect2->setRange(0.000001, 100);
    m_poFInitialMeshResolutionVect2->setValue(1);
    m_poFInitialMeshResolutionVect3 = new medDoubleParameter("ZMeshResolution", this);
    m_poFInitialMeshResolutionVect3->setCaption("Spline Mesh resolution for Z");
    m_poFInitialMeshResolutionVect3->setDescription("Mesh resolution for Z");
    m_poFInitialMeshResolutionVect3->setRange(0.000001, 100);
    m_poFInitialMeshResolutionVect3->setValue(1);
}

medItkBiasCorrectionProcess::~medItkBiasCorrectionProcess()
{

}


QString medItkBiasCorrectionProcess::caption() const
{
    return "Bias correction";
}

QString medItkBiasCorrectionProcess::description() const
{
    return "Use ITK N4BiasCorrectionFilter to compute a bias corrected image.";
}

medAbstractJob::medJobExitStatus medItkBiasCorrectionProcess::run()
{
    medAbstractJob::medJobExitStatus jobExitSatus = medAbstractJob::MED_JOB_EXIT_FAILURE;

    if(this->input())
    {
        QString id =  this->input()->identifier();

        if ( id == "itkDataImageChar3" )
        {
            jobExitSatus = this->_run<char, 3>();
        }
        else if ( id == "itkDataImageUChar3" )
        {
            jobExitSatus = this->_run<unsigned char, 3>();
        }
        else if ( id == "itkDataImageShort3" )
        {
            jobExitSatus = this->_run<short, 3>();
        }
        else if ( id == "itkDataImageUShort3" )
        {
            jobExitSatus = this->_run<unsigned short, 3>();
        }
        else if ( id == "itkDataImageInt3" )
        {
            jobExitSatus = this->_run<int, 3>();
        }
        else if ( id == "itkDataImageUInt3" )
        {
            jobExitSatus = this->_run<unsigned int, 3>();
        }
        else if ( id == "itkDataImageLong3" )
        {
            jobExitSatus = this->_run<long, 3>();
        }
        else if ( id== "itkDataImageULong3" )
        {
            jobExitSatus = this->_run<unsigned long, 3>();
        }
        else if ( id == "itkDataImageFloat3" )
        {
            jobExitSatus = this->_run<float, 3>();
        }
        else if ( id == "itkDataImageDouble3" )
        {
            jobExitSatus = this->_run<double, 3>();
        }
    }
    return jobExitSatus;
}


template <class inputType, unsigned int Dimension> medAbstractJob::medJobExitStatus medItkBiasCorrectionProcess::_run()
{
    medJobExitStatus eRes = medAbstractJob::MED_JOB_EXIT_SUCCESS;
    
    try
    {
       eRes = N4BiasCorrectionCore<inputType, Dimension>();
    }
    catch (...)
    {
       eRes = medAbstractJob::MED_JOB_EXIT_FAILURE;
    }


    return eRes;
}

void medItkBiasCorrectionProcess::cancel()
{
    if(this->isRunning())
    {
        m_bAborting = true;
        m_filter->AbortGenerateDataOn();
    }
}

#define ABORT_CHECKING(x) {if (x) {m_bAborting = false; return medAbstractJob::MED_JOB_EXIT_CANCELLED;}}

template <class inputType, unsigned int Dimension> medAbstractJob::medJobExitStatus medItkBiasCorrectionProcess::N4BiasCorrectionCore()
{
    medJobExitStatus eRes = medAbstractJob::MED_JOB_EXIT_SUCCESS;

    typedef itk::Image<inputType, Dimension > ImageType;
    typedef itk::Image <float, Dimension> OutputImageType;
    typedef itk::Image<unsigned char, Dimension> MaskImageType;
    typedef itk::N4BiasFieldCorrectionImageFilter<OutputImageType, MaskImageType, OutputImageType> BiasFilter;
    typedef itk::ConstantPadImageFilter<OutputImageType, OutputImageType> PadderType;
    typedef itk::ConstantPadImageFilter<MaskImageType, MaskImageType> MaskPadderType;
    typedef itk::ShrinkImageFilter<OutputImageType, OutputImageType> ShrinkerType;
    typedef itk::ShrinkImageFilter<MaskImageType, MaskImageType> MaskShrinkerType;
    typedef itk::BSplineControlPointImageFilter<typename BiasFilter::BiasFieldControlPointLatticeType, typename BiasFilter::ScalarImageType> BSplinerType;
    typedef itk::ExpImageFilter<OutputImageType, OutputImageType> ExpFilterType;
    typedef itk::DivideImageFilter<OutputImageType, OutputImageType, OutputImageType> DividerType;
    typedef itk::ExtractImageFilter<OutputImageType, OutputImageType> CropperType;

    unsigned int uiThreadNb = static_cast<unsigned int>(m_poUIThreadNb->value());
    unsigned int uiShrinkFactors = static_cast<unsigned int>(m_poUIShrinkFactors->value());
    unsigned int uiSplineOrder = static_cast<unsigned int>(m_poUISplineOrder->value());
    float fWienerFilterNoise = static_cast<float>(m_poFWienerFilterNoise->value());
    float fbfFWHM = static_cast<float>(m_poFbfFWHM->value());
    float fConvergenceThreshold = static_cast<float>(m_poFConvergenceThreshold->value());
    float fSplineDistance = static_cast<float>(m_poFSplineDistance->value());

    float fProgression = 0;

    QStringList oListValue = m_poSMaxIterations->value().split("x");

    std::vector<unsigned int> oMaxNumbersIterationsVector(oListValue.size());
    std::vector<float> oInitialMeshResolutionVect(Dimension);
    for (int i=0; i < static_cast<int>(oMaxNumbersIterationsVector.size()); ++i)
    {
       oMaxNumbersIterationsVector[i] = (unsigned int)oListValue[i].toInt();
    }
    oInitialMeshResolutionVect[0] = static_cast<float>(m_poFInitialMeshResolutionVect1->value());
    oInitialMeshResolutionVect[1] = static_cast<float>(m_poFInitialMeshResolutionVect2->value());
    oInitialMeshResolutionVect[2] = static_cast<float>(m_poFInitialMeshResolutionVect3->value());

    typename ImageType::Pointer image = dynamic_cast<ImageType *>((itk::Object*)(this->input()->data()));
    typedef itk::CastImageFilter <ImageType, OutputImageType> CastFilterType;
    typename CastFilterType::Pointer castFilter = CastFilterType::New();
    castFilter->SetInput(image);

    /********************************************************************************/
    /***************************** PREPARING STARTING *******************************/
    /********************************************************************************/

    /*** 0 ******************* Create filter and accessories ******************/
    ABORT_CHECKING(m_bAborting);
    typename BiasFilter::Pointer filter = BiasFilter::New();
    typename BiasFilter::ArrayType oNumberOfControlPointsArray;
    m_filter = filter;

    /*** 1 ******************* Read input image *******************************/
    ABORT_CHECKING(m_bAborting);
    fProgression = 1;
    updateProgression(fProgression);

    /*** 2 ******************* Creating Otsu mask *****************************/
    ABORT_CHECKING(m_bAborting);
    itk::TimeProbe timer;
    timer.Start();
    typename MaskImageType::Pointer maskImage = nullptr;
    typedef itk::OtsuThresholdImageFilter<OutputImageType, MaskImageType> ThresholderType;
    typename ThresholderType::Pointer otsu = ThresholderType::New();
    m_filter = otsu;
    otsu->SetInput(castFilter->GetOutput());
    otsu->SetNumberOfHistogramBins(200);
    otsu->SetInsideValue(0);
    otsu->SetOutsideValue(1);
    otsu->SetNumberOfWorkUnits(uiThreadNb);
    otsu->Update();
    updateProgression(fProgression);
    maskImage = otsu->GetOutput();


    /*** 3A *************** Set Maximum number of Iterations for the filter ***/
    ABORT_CHECKING(m_bAborting);
    typename BiasFilter::VariableSizeArrayType itkTabMaximumIterations;
    itkTabMaximumIterations.SetSize(oMaxNumbersIterationsVector.size());
    for (int i = 0; i < static_cast<int>(oMaxNumbersIterationsVector.size()); ++i)
    {
        itkTabMaximumIterations[i] = oMaxNumbersIterationsVector[i];
    }
    filter->SetMaximumNumberOfIterations(itkTabMaximumIterations);

    /*** 3B *************** Set Fitting Levels for the filter *****************/
    typename BiasFilter::ArrayType oFittingLevelsTab;
    oFittingLevelsTab.Fill(oMaxNumbersIterationsVector.size());
    filter->SetNumberOfFittingLevels(oFittingLevelsTab);

    updateProgression(fProgression);

    /*** 4 ******************* Save image's index, size, origine **************/
    ABORT_CHECKING(m_bAborting);
    typename ImageType::IndexType oImageIndex = image->GetLargestPossibleRegion().GetIndex();
    typename ImageType::SizeType oImageSize = image->GetLargestPossibleRegion().GetSize();
    typename ImageType::PointType newOrigin = image->GetOrigin();

    typename OutputImageType::Pointer outImage = castFilter->GetOutput();

    if (fSplineDistance > 0)
    {
        /*** 5 ******************* Compute number of control points  **************/
        ABORT_CHECKING(m_bAborting);
        itk::SizeValueType lowerBound[3];
        itk::SizeValueType upperBound[3];

        for (unsigned int i = 0; i < 3; i++)
        {
            float domain = static_cast<float>(image->GetLargestPossibleRegion().GetSize()[i] - 1) * image->GetSpacing()[i];
            unsigned int numberOfSpans = static_cast<unsigned int>(std::ceil(domain / fSplineDistance));
            unsigned long extraPadding = static_cast<unsigned long>((numberOfSpans * fSplineDistance - domain) / image->GetSpacing()[i] + 0.5);
            lowerBound[i] = static_cast<unsigned long>(0.5 * extraPadding);
            upperBound[i] = extraPadding - lowerBound[i];
            newOrigin[i] -= (static_cast<float>(lowerBound[i]) * image->GetSpacing()[i]);
            oNumberOfControlPointsArray[i] = numberOfSpans + filter->GetSplineOrder();
        }
        updateProgression(fProgression);

        /*** 6 ******************* Padder  ****************************************/
        ABORT_CHECKING(m_bAborting);
        typename PadderType::Pointer imagePadder = PadderType::New();
        m_filter = imagePadder;
        imagePadder->SetInput(castFilter->GetOutput());
        imagePadder->SetPadLowerBound(lowerBound);
        imagePadder->SetPadUpperBound(upperBound);
        imagePadder->SetConstant(0);
        imagePadder->SetNumberOfWorkUnits(uiThreadNb);
        imagePadder->Update();
        updateProgression(fProgression);

        outImage = imagePadder->GetOutput();

        /*** 7 ******************** Handle the mask image *************************/
        ABORT_CHECKING(m_bAborting);
        typename MaskPadderType::Pointer maskPadder = MaskPadderType::New();
        m_filter = maskPadder;
        maskPadder->SetInput(maskImage);
        maskPadder->SetPadLowerBound(lowerBound);
        maskPadder->SetPadUpperBound(upperBound);
        maskPadder->SetConstant(0);
        maskPadder->SetNumberOfWorkUnits(uiThreadNb);
        maskPadder->Update();
        updateProgression(fProgression);

        maskImage = maskPadder->GetOutput();

        /*** 8 ******************** SetNumber Of Control Points *******************/
        ABORT_CHECKING(m_bAborting);
        filter->SetNumberOfControlPoints(oNumberOfControlPointsArray);
    }
    else if (oInitialMeshResolutionVect.size() == 3)
    {
        /*** 9 ******************** SetNumber Of Control Points alternative *******/
        ABORT_CHECKING(m_bAborting);
        for (unsigned i = 0; i < 3; i++)
        {
            oNumberOfControlPointsArray[i] = static_cast<unsigned int>(oInitialMeshResolutionVect[i]) + filter->GetSplineOrder();
        }
        filter->SetNumberOfControlPoints(oNumberOfControlPointsArray);

        updateProgression(fProgression, 3);
    }
    else
    {
        fProgression = 0;
        updateProgression(fProgression);
        std::cout << "No BSpline distance and Mesh Resolution is ignored because not 3 dimensions" << std::endl;
    }

    /*** 10 ******************* Shrinker image ********************************/
    ABORT_CHECKING(m_bAborting);
    typename ShrinkerType::Pointer imageShrinker = ShrinkerType::New();
    m_filter = imageShrinker;
    imageShrinker->SetInput(outImage);

    /*** 11 ******************* Shrinker mask *********************************/
    ABORT_CHECKING(m_bAborting);
    typename MaskShrinkerType::Pointer maskShrinker = MaskShrinkerType::New();
    m_filter = maskShrinker;
    maskShrinker->SetInput(maskImage);

    /*** 12 ******************* Shrink mask and image *************************/
    ABORT_CHECKING(m_bAborting);
    imageShrinker->SetShrinkFactors(uiShrinkFactors);
    maskShrinker->SetShrinkFactors(uiShrinkFactors);
    imageShrinker->SetNumberOfWorkUnits(uiThreadNb);
    maskShrinker->SetNumberOfWorkUnits(uiThreadNb);
    imageShrinker->Update();
    updateProgression(fProgression);
    maskShrinker->Update();
    updateProgression(fProgression);

    /*** 13 ******************* Filter setings ********************************/
    ABORT_CHECKING(m_bAborting);
    filter->SetSplineOrder(uiSplineOrder);
    filter->SetWienerFilterNoise(fWienerFilterNoise);
    filter->SetBiasFieldFullWidthAtHalfMaximum(fbfFWHM);
    filter->SetConvergenceThreshold(fConvergenceThreshold);
    filter->SetInput(imageShrinker->GetOutput());
    filter->SetMaskImage(maskShrinker->GetOutput());

    /*** 14 ******************* Apply filter **********************************/
    ABORT_CHECKING(m_bAborting);
    try
    {
        filter->SetNumberOfWorkUnits(uiThreadNb);
        filter->Update();
        updateProgression(fProgression, 5);
    }
    catch (itk::ExceptionObject & err)
    {
        std::cerr << "ExceptionObject caught !" << std::endl;
        std::cerr << err << std::endl;
        eRes = medAbstractJob::MED_JOB_EXIT_FAILURE;
        return eRes;
    }


    /**
    * Reconstruct the bias field at full image resolution.  Divide
    * the original input image by the bias field to get the final
    * corrected image.
    */
    ABORT_CHECKING(m_bAborting);
    typename BSplinerType::Pointer bspliner = BSplinerType::New();
    m_filter = bspliner;
    bspliner->SetInput(filter->GetLogBiasFieldControlPointLattice());
    bspliner->SetSplineOrder(filter->GetSplineOrder());
    bspliner->SetSize(image->GetLargestPossibleRegion().GetSize());
    bspliner->SetOrigin(newOrigin);
    bspliner->SetDirection(image->GetDirection());
    bspliner->SetSpacing(image->GetSpacing());
    bspliner->SetNumberOfWorkUnits(uiThreadNb);
    bspliner->Update();
    updateProgression(fProgression);


    /*********************** Logarithm phase ***************************/
    ABORT_CHECKING(m_bAborting);
    typename OutputImageType::Pointer logField = OutputImageType::New();
    logField->SetOrigin(image->GetOrigin());
    logField->SetSpacing(image->GetSpacing());
    logField->SetRegions(image->GetLargestPossibleRegion());
    logField->SetDirection(image->GetDirection());
    logField->Allocate();

    itk::ImageRegionIterator<typename BiasFilter::ScalarImageType> IB(bspliner->GetOutput(), bspliner->GetOutput()->GetLargestPossibleRegion());

    itk::ImageRegionIterator<OutputImageType> IF(logField, logField->GetLargestPossibleRegion());

    for (IB.GoToBegin(), IF.GoToBegin(); !IB.IsAtEnd(); ++IB, ++IF)
    {
        IF.Set(IB.Get()[0]);
    }


    /*********************** Exponential phase *************************/
    ABORT_CHECKING(m_bAborting);
    typename ExpFilterType::Pointer expFilter = ExpFilterType::New();
    m_filter = expFilter;
    expFilter->SetInput(logField);
    expFilter->SetNumberOfWorkUnits(uiThreadNb);
    expFilter->Update();
    updateProgression(fProgression);

    /************************ Dividing phase ***************************/
    ABORT_CHECKING(m_bAborting);
    typename DividerType::Pointer divider = DividerType::New();
    m_filter = divider;
    divider->SetInput1(castFilter->GetOutput());
    divider->SetInput2(expFilter->GetOutput());
    divider->SetNumberOfWorkUnits(uiThreadNb);
    divider->Update();
    updateProgression(fProgression);


    /******************** Prepare cropping phase ***********************/
    ABORT_CHECKING(m_bAborting);
    typename ImageType::RegionType inputRegion;
    inputRegion.SetIndex(oImageIndex);
    inputRegion.SetSize(oImageSize);

    /************************ Cropping phase ***************************/
    ABORT_CHECKING(m_bAborting);
    typename CropperType::Pointer cropper = CropperType::New();
    m_filter = cropper;
    cropper->SetInput(divider->GetOutput());
    cropper->SetExtractionRegion(inputRegion);
    cropper->SetDirectionCollapseToSubmatrix();
    cropper->SetNumberOfWorkUnits(uiThreadNb);
    cropper->Update();
    updateProgression(fProgression);

    /********************** Write output image *************************/
    ABORT_CHECKING(m_bAborting);
    medAbstractImageData *out = qobject_cast<medAbstractImageData *>(medAbstractDataFactory::instance()->create("itkDataImageFloat3"));
    out->setData(cropper->GetOutput());
    this->setOutput(out);

    m_filter = 0;
    
    return eRes;
}

float medItkBiasCorrectionProcess::updateProgression(float &pio_rfProgression, int pi_iStepLevel)
{
    pio_rfProgression > 0 ? pio_rfProgression+= pi_iStepLevel : 0;
    progression()->setValue(pio_rfProgression * 100 / NB_STEPS);
    return pio_rfProgression;
}
