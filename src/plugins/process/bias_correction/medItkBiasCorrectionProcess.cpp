/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkBiasCorrectionProcess.h>

#include <dtkLog>

#include <itkImage.h>
//#include <itkBiasCorrectionFilter.h>
#include <itkBinaryBallStructuringElement.h>
#include <itkCommand.h>

#include <medAbstractImageData.h>
#include <medAbstractDataFactory.h>
#include <medIntParameter.h>

#include <itkN4BiasFieldCorrectionImageFilter.h>
#include <itkExtractImageFilter.h>
#include <itkConstantPadImageFilter.h>
#include <itkOtsuThresholdImageFilter.h>
#include <itkShrinkImageFilter.h>
#include <itkTimeProbe.h>


#include <medDoubleParameter.h>

#define MAX_INT_POSITIVE 4294967295


medItkBiasCorrectionProcess::medItkBiasCorrectionProcess(QObject *parent): medAbstractBiasCorrectionProcess(parent)
{
    m_filter = NULL;


    m_poUIThreadNb = new medIntParameter("ThreadNb", this);
    m_poUIThreadNb->setCaption("Number of threads");
    m_poUIThreadNb->setDescription("Set the number of computing treads");
    m_poUIThreadNb->setRange(1, itk::MultiThreader::GetGlobalDefaultNumberOfThreads());
    m_poUIThreadNb->setValue(itk::MultiThreader::GetGlobalDefaultNumberOfThreads());

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

    m_poUIMaxNumbersIterationsVector1 = new medIntParameter("Iterations1st", this);
    m_poUIMaxNumbersIterationsVector1->setCaption("Iterations vector");
    m_poUIMaxNumbersIterationsVector1->setDescription("Firsts iterations round");
    m_poUIMaxNumbersIterationsVector1->setRange(0, 500);
    m_poUIMaxNumbersIterationsVector1->setValue(50);
    m_poUIMaxNumbersIterationsVector2 = new medIntParameter("Iterations2nd", this);
    m_poUIMaxNumbersIterationsVector2->setCaption("Iterations vector");
    m_poUIMaxNumbersIterationsVector2->setDescription("Seconds iterations round");
    m_poUIMaxNumbersIterationsVector2->setRange(0, 500);
    m_poUIMaxNumbersIterationsVector2->setValue(40);
    m_poUIMaxNumbersIterationsVector3 = new medIntParameter("Iterations3rd", this);
    m_poUIMaxNumbersIterationsVector3->setCaption("Iterations vector");
    m_poUIMaxNumbersIterationsVector3->setDescription("Thirds iterations round");
    m_poUIMaxNumbersIterationsVector3->setRange(0, 500);
    m_poUIMaxNumbersIterationsVector3->setValue(30);

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
    m_poFInitialMeshResolutionVect1->setCaption("Spline grid resolution");
    m_poFInitialMeshResolutionVect1->setDescription("Mesh resolution for X");
    m_poFInitialMeshResolutionVect1->setRange(0.000001, MAX_INT_POSITIVE);
    m_poFInitialMeshResolutionVect1->setValue(1);
    m_poFInitialMeshResolutionVect2 = new medDoubleParameter("YMeshResolution", this);
    m_poFInitialMeshResolutionVect2->setCaption("Spline grid resolution");
    m_poFInitialMeshResolutionVect2->setDescription("Mesh resolution for Y");
    m_poFInitialMeshResolutionVect2->setRange(0.000001, MAX_INT_POSITIVE);
    m_poFInitialMeshResolutionVect2->setValue(1);
    m_poFInitialMeshResolutionVect3 = new medDoubleParameter("ZMeshResolution", this);
    m_poFInitialMeshResolutionVect3->setCaption("Spline grid resolution");
    m_poFInitialMeshResolutionVect3->setDescription("Mesh resolution for Z");
    m_poFInitialMeshResolutionVect3->setRange(0.000001, MAX_INT_POSITIVE);
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
       eRes = N4BiasCorrectionCore();
    }
    catch (...)
    {
       eRes = medAbstractJob::MED_JOB_EXIT_FAILURE;
    }


    return eRes;
}

void medItkBiasCorrectionProcess::cancel()
{
    if(this->isRunning() && m_filter.IsNotNull())
    {
        m_filter->AbortGenerateDataOn();
    }
}

medAbstractJob::medJobExitStatus medItkBiasCorrectionProcess::N4BiasCorrectionCore()
{
   medJobExitStatus eRes = medAbstractJob::MED_JOB_EXIT_SUCCESS;

   typedef itk::Image<float, 3 > ImageType;
   typedef itk::Image<unsigned char, 3> MaskImageType;
   typedef itk::N4BiasFieldCorrectionImageFilter<ImageType, MaskImageType, ImageType> BiasFilter;
   typedef itk::ConstantPadImageFilter<ImageType, ImageType> PadderType;
   typedef itk::ConstantPadImageFilter<MaskImageType, MaskImageType> MaskPadderType;
   typedef itk::ShrinkImageFilter<ImageType, ImageType> ShrinkerType;
   typedef itk::ShrinkImageFilter<MaskImageType, MaskImageType> MaskShrinkerType;
   typedef itk::BSplineControlPointImageFilter< BiasFilter::BiasFieldControlPointLatticeType, BiasFilter::ScalarImageType> BSplinerType;
   typedef itk::ExpImageFilter<ImageType, ImageType> ExpFilterType;
   typedef itk::DivideImageFilter<ImageType, ImageType, ImageType> DividerType;
   typedef itk::ExtractImageFilter<ImageType, ImageType> CropperType;


   unsigned int uiThreadNb = static_cast<unsigned int>(m_poUIThreadNb->value());
   unsigned int uiShrinkFactors = static_cast<unsigned int>(m_poUIShrinkFactors->value());
   unsigned int uiSplineOrder = static_cast<unsigned int>(m_poUISplineOrder->value());
   float fWienerFilterNoise = static_cast<float>(m_poFWienerFilterNoise->value());
   float fbfFWHM = static_cast<float>(m_poFbfFWHM->value());
   float fConvergenceThreshold = static_cast<float>(m_poFConvergenceThreshold->value());
   float fSplineDistance = static_cast<float>(m_poFSplineDistance->value());

   std::vector<unsigned int> oMaxNumbersIterationsVector;
   std::vector<float> oInitialMeshResolutionVect;
   oMaxNumbersIterationsVector[0] = static_cast<unsigned int>(m_poUIMaxNumbersIterationsVector1->value());
   oMaxNumbersIterationsVector[1] = static_cast<unsigned int>(m_poUIMaxNumbersIterationsVector2->value());
   oMaxNumbersIterationsVector[2] = static_cast<unsigned int>(m_poUIMaxNumbersIterationsVector3->value());
   oInitialMeshResolutionVect[0] = static_cast<float>(m_poFInitialMeshResolutionVect1->value());
   oInitialMeshResolutionVect[1] = static_cast<float>(m_poFInitialMeshResolutionVect2->value());
   oInitialMeshResolutionVect[2] = static_cast<float>(m_poFInitialMeshResolutionVect3->value());

   /********************************************************************************/
   /***************************** PREPARING STARTING *******************************/
   /********************************************************************************/

   /*** 0 ******************* Create filter and accessories ******************/
   BiasFilter::Pointer filter = BiasFilter::New();
   BiasFilter::ArrayType oNumberOfControlPointsArray;

   /*** 1 ******************* Read input image *******************************/
   ImageType::Pointer image = dynamic_cast<ImageType *>((itk::Object*)(this->input()->data()));

   /*** 2 ******************* Creating Otsu mask *****************************/
   itk::TimeProbe timer;
   timer.Start();
   MaskImageType::Pointer maskImage = ITK_NULLPTR;
   typedef itk::OtsuThresholdImageFilter<ImageType, MaskImageType> ThresholderType;
   ThresholderType::Pointer otsu = ThresholderType::New();
   otsu->SetInput(image);
   otsu->SetNumberOfHistogramBins(200);
   otsu->SetInsideValue(0);
   otsu->SetOutsideValue(1);

   otsu->SetNumberOfThreads(uiThreadNb);
   otsu->Update();
   maskImage = otsu->GetOutput();

   /*** 3A *************** Set Maximum number of Iterations for the filter ***/
   BiasFilter::VariableSizeArrayType itkTabMaximumIterations;
   itkTabMaximumIterations.SetSize(oMaxNumbersIterationsVector.size());
   for (int i = 0; i < oMaxNumbersIterationsVector.size(); ++i)
   {
      itkTabMaximumIterations[i] = oMaxNumbersIterationsVector[i];
   }
   filter->SetMaximumNumberOfIterations(itkTabMaximumIterations);

   /*** 3B *************** Set Fitting Levels for the filter *****************/
   BiasFilter::ArrayType oFittingLevelsTab;
   oFittingLevelsTab.Fill(oMaxNumbersIterationsVector.size());
   filter->SetNumberOfFittingLevels(oFittingLevelsTab);

   /*** 4 ******************* Save image's index, size, origine **************/
   ImageType::IndexType oImageIndex = image->GetLargestPossibleRegion().GetIndex();
   ImageType::SizeType oImageSize = image->GetLargestPossibleRegion().GetSize();
   ImageType::PointType newOrigin = image->GetOrigin();

   if (fSplineDistance > 0)
   {
      /*** 5 ******************* Compute number of control points  **************/
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

      /*** 6 ******************* Padder  ****************************************/
      PadderType::Pointer imagePadder = PadderType::New();
      imagePadder->SetInput(image);
      imagePadder->SetPadLowerBound(lowerBound);
      imagePadder->SetPadUpperBound(upperBound);
      imagePadder->SetConstant(0);
      imagePadder->SetNumberOfThreads(uiThreadNb);
      imagePadder->Update();

      image = imagePadder->GetOutput();

      /*** 7 ******************** Handle the mask image *************************/
      MaskPadderType::Pointer maskPadder = MaskPadderType::New();
      maskPadder->SetInput(maskImage);
      maskPadder->SetPadLowerBound(lowerBound);
      maskPadder->SetPadUpperBound(upperBound);
      maskPadder->SetConstant(0);

      maskPadder->SetNumberOfThreads(uiThreadNb);
      maskPadder->Update();

      maskImage = maskPadder->GetOutput();

      /*** 8 ******************** SetNumber Of Control Points *******************/
      filter->SetNumberOfControlPoints(oNumberOfControlPointsArray);
   }
   else if (oInitialMeshResolutionVect.size() == 3)
   {
      /*** 9 ******************** SetNumber Of Control Points alternative *******/
      for (unsigned i = 0; i < 3; i++)
      {
         oNumberOfControlPointsArray[i] = static_cast<unsigned int>(oInitialMeshResolutionVect[i]) + filter->GetSplineOrder();
      }
      filter->SetNumberOfControlPoints(oNumberOfControlPointsArray);
   }
   else
   {
      std::cout << "No BSpline distance and Mesh Resolution is ignored because not 3 dimensions" << std::endl;
   }

   /*** 10 ******************* Shrinker image ********************************/
   ShrinkerType::Pointer imageShrinker = ShrinkerType::New();
   imageShrinker->SetInput(image);

   /*** 11 ******************* Shrinker mask *********************************/
   MaskShrinkerType::Pointer maskShrinker = MaskShrinkerType::New();
   maskShrinker->SetInput(maskImage);

   /*** 12 ******************* Shrink mask and image *************************/
   imageShrinker->SetShrinkFactors(uiShrinkFactors);
   maskShrinker->SetShrinkFactors(uiShrinkFactors);
   imageShrinker->SetNumberOfThreads(uiThreadNb);
   maskShrinker->SetNumberOfThreads(uiThreadNb);
   imageShrinker->Update();
   maskShrinker->Update();


   /*** 13 ******************* Filter setings ********************************/
   filter->SetSplineOrder(uiSplineOrder);
   filter->SetWienerFilterNoise(fWienerFilterNoise);
   filter->SetBiasFieldFullWidthAtHalfMaximum(fbfFWHM);
   filter->SetConvergenceThreshold(fConvergenceThreshold);
   filter->SetInput(imageShrinker->GetOutput());
   filter->SetMaskImage(maskShrinker->GetOutput());

   /*** 14 ******************* Apply filter **********************************/
   itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
   callback->SetCallback(eventCallback);
   filter->AddObserver(itk::ProgressEvent(), callback);
   try
   {
      filter->SetNumberOfThreads(uiThreadNb);
      filter->Update();
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
   BSplinerType::Pointer bspliner = BSplinerType::New();
   bspliner->SetInput(filter->GetLogBiasFieldControlPointLattice());
   bspliner->SetSplineOrder(filter->GetSplineOrder());
   bspliner->SetSize(image->GetLargestPossibleRegion().GetSize());
   bspliner->SetOrigin(newOrigin);
   bspliner->SetDirection(image->GetDirection());
   bspliner->SetSpacing(image->GetSpacing());
   bspliner->SetNumberOfThreads(uiThreadNb);
   bspliner->Update();

   ImageType::Pointer logField = ImageType::New();
   logField->SetOrigin(image->GetOrigin());
   logField->SetSpacing(image->GetSpacing());
   logField->SetRegions(image->GetLargestPossibleRegion());
   logField->SetDirection(image->GetDirection());
   logField->Allocate();

   itk::ImageRegionIterator<BiasFilter::ScalarImageType> IB(bspliner->GetOutput(), bspliner->GetOutput()->GetLargestPossibleRegion());

   itk::ImageRegionIterator<ImageType> IF(logField, logField->GetLargestPossibleRegion());

   for (IB.GoToBegin(), IF.GoToBegin(); !IB.IsAtEnd(); ++IB, ++IF)
   {
      IF.Set(IB.Get()[0]);
   }

   ExpFilterType::Pointer expFilter = ExpFilterType::New();
   expFilter->SetInput(logField);
   expFilter->SetNumberOfThreads(uiThreadNb);
   expFilter->Update();

   DividerType::Pointer divider = DividerType::New();
   divider->SetInput1(image);
   divider->SetInput2(expFilter->GetOutput());
   divider->SetNumberOfThreads(uiThreadNb);
   divider->Update();

   ImageType::RegionType inputRegion;
   inputRegion.SetIndex(oImageIndex);
   inputRegion.SetSize(oImageSize);

   CropperType::Pointer cropper = CropperType::New();
   cropper->SetInput(divider->GetOutput());
   cropper->SetExtractionRegion(inputRegion);
   cropper->SetDirectionCollapseToSubmatrix();
   cropper->SetNumberOfThreads(uiThreadNb);
   cropper->Update();

   //timer.Stop();
   //std::cout << "\nComputation time : " << timer.GetTotal() << std::endl;

   /********************** Write output image *************************/
   medAbstractImageData *out = qobject_cast<medAbstractImageData *>(medAbstractDataFactory::instance()->create(this->input()->identifier()));
   out->setData(cropper->GetOutput());
   this->setOutput(out);
}
