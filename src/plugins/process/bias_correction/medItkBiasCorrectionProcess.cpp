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
medItkBiasCorrectionProcess::medItkBiasCorrectionProcess(QObject *parent)
    : medAbstractBiasCorrectionProcess(parent)

{
    m_filter = NULL;
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
        }/*
        else if ( id == "itkDataImageChar4" )
        {
            jobExitSatus = this->_run<char, 4>();
        }
        else if ( id == "itkDataImageUChar4" )
        {
            jobExitSatus = this->_run<unsigned char, 4>();
        }
        else if ( id == "itkDataImageShort4" )
        {
            jobExitSatus = this->_run<short, 4>();
        }
        else if ( id == "itkDataImageUShort4" )
        {
            jobExitSatus = this->_run<unsigned short, 4>();
        }
        else if ( id == "itkDataImageInt4" )
        {
            jobExitSatus = this->_run<int, 4>();
        }
        else if ( id == "itkDataImageUInt4" )
        {
            jobExitSatus = this->_run<unsigned int, 4>();
        }
        else if ( id == "itkDataImageLong4" )
        {
            jobExitSatus = this->_run<long, 4>();
        }
        else if ( id == "itkDataImageULong4" )
        {
            jobExitSatus = this->_run<unsigned long, 4>();
        }
        else if ( id == "itkDataImageFloat4" )
        {
            jobExitSatus = this->_run<float, 4>();
        }
        else if ( id == "itkDataImageDouble4" )
        {
            jobExitSatus = this->_run<double, 4>();
        }*/
    }
    return jobExitSatus;
}


template <class inputType, unsigned int Dimension> medAbstractJob::medJobExitStatus medItkBiasCorrectionProcess::_run()
{
    N4BiasCorrectionCore();
    return medAbstractJob::MED_JOB_EXIT_FAILURE;
}

void medItkBiasCorrectionProcess::cancel()
{
    if(this->isRunning() && m_filter.IsNotNull())
    {
        m_filter->AbortGenerateDataOn();
    }
}

void medItkBiasCorrectionProcess::N4BiasCorrectionCore()
{
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

   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////  TODO TODO TODO ////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////  TODO TODO TODO ////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////  TODO TODO TODO ////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   unsigned int uiThreadNb = 0;
   unsigned int uiShrinkFactors = 0;
   unsigned int uiSplineOrder = 0;
   float fWienerFilterNoise;
   float fbfFWHM;
   float fConvergenceThreshold;
   std::vector<unsigned int> oMaxNumbersIterationsVector;//TODO = extractMaxNumberOfIterationsVector(oArgIterationsString.getValue());
   std::vector<float> oInitialMeshResolutionVect;//TODO = ConvertVector<float>(oArgInitialMeshResolutionString.getValue());
   float fSplineDistance;//TODO = oArgSplineDistance.getValue();


   /********************************************************************************/
   /***************************** PREPARING STARTING *******************************/
   /********************************************************************************/

   /*** 0 ******************* Create filter and accessories ******************/
   BiasFilter::Pointer filter = BiasFilter::New();
   BiasFilter::ArrayType oNumberOfControlPointsArray;

   /*** 1 ******************* Read input image *******************************/
   ImageType::Pointer image = dynamic_cast<ImageType *>((itk::Object*)(this->input()->data()));

   /*** 2 ******************* Creating Otsu mask *****************************/
   //TODO std::cout << "Creating Otsu mask." << std::endl;
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
   imageShrinker->SetShrinkFactors(1);

   /*** 11 ******************* Shrinker mask *********************************/
   MaskShrinkerType::Pointer maskShrinker = MaskShrinkerType::New();
   maskShrinker->SetInput(maskImage);
   maskShrinker->SetShrinkFactors(1);

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
      exit(EXIT_FAILURE);
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

   timer.Stop();
   std::cout << "\nComputation time : " << timer.GetTotal() << std::endl;

   /********************** Write output image *************************/
   medAbstractImageData *out = qobject_cast<medAbstractImageData *>(medAbstractDataFactory::instance()->create(this->input()->identifier()));
   out->setData(cropper->GetOutput());
   this->setOutput(out);
}
