#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medProcessPresenterFactory.h>

#include <medAbstractOpeningImageProcessPresenter.h>
#include <medAbstractClosingImageProcessPresenter.h>
#include <medAbstractDilateImageProcessPresenter.h>
#include <medAbstractErodeImageProcessPresenter.h>

#include <medAbstractAddImageProcessPresenter.h>
#include <medAbstractSubtractImageProcessPresenter.h>
#include <medAbstractMultiplyImageProcessPresenter.h>
#include <medAbstractDivideImageProcessPresenter.h>

#include <medAbstractMaskImageProcessPresenter.h>
#include <medAbstractDWIMaskingProcessPresenter.h>
#include <medAbstractDiffusionModelEstimationProcessPresenter.h>
#include <medAbstractDiffusionScalarMapsProcessPresenter.h>
#include <medAbstractTractographyProcessPresenter.h>

#include <medAbstractSingleFilterOperationProcessPresenter.h>
#include <medAbstractAddFilterProcessPresenter.h>
#include <medAbstractDivideFilterProcessPresenter.h>
#include <medAbstractGaussianFilterProcessPresenter.h>
#include <medAbstractInvertFilterProcessPresenter.h>
#include <medAbstractMedianFilterProcessPresenter.h>
#include <medAbstractMultiplyFilterProcessPresenter.h>
#include <medAbstractNormalizeFilterProcessPresenter.h>
#include <medAbstractShrinkFilterProcessPresenter.h>
#include <medAbstractSubtractFilterProcessPresenter.h>
#include <medAbstractWindowingFilterProcessPresenter.h>
#include <medAbstractImageDenoisingProcessPresenter.h>
#include <medAbstractRelaxometryEstimationProcessPresenter.h>
#include <medAbstractSymmetryPlaneAlignmentProcessPresenter.h>
#include <medAbstractBiasCorrectionProcessPresenter.h>

#include <medCoreGuiExport.h>

namespace medWidgets
{
    namespace pluginManager
    {
        MEDCOREGUI_EXPORT void initialize(const QString& path = QString(), bool verbose = true);
    }

    namespace generic
    {
        MEDCOREGUI_EXPORT medAbstractProcessPresenterFactory& presenterFactory();
    }

    namespace morphomathOperation
    {
        namespace erodeImage
        {
            MEDCOREGUI_EXPORT medAbstractErodeImageProcessPresenterFactory& presenterFactory();
        }
        namespace dilateImage
        {
            MEDCOREGUI_EXPORT medAbstractDilateImageProcessPresenterFactory& presenterFactory();
        }
        namespace openingImage
        {
            MEDCOREGUI_EXPORT medAbstractOpeningImageProcessPresenterFactory& presenterFactory();
        }
        namespace closingImage
        {
            MEDCOREGUI_EXPORT medAbstractClosingImageProcessPresenterFactory& presenterFactory();
        }
    }

    namespace arithmeticOperation
    {
        namespace addImage
        {
            MEDCOREGUI_EXPORT medAbstractAddImageProcessPresenterFactory& presenterFactory();
        }

        namespace subtractImage
        {
            MEDCOREGUI_EXPORT medAbstractSubtractImageProcessPresenterFactory& presenterFactory();
        }

        namespace multiplyImage
        {
            MEDCOREGUI_EXPORT medAbstractMultiplyImageProcessPresenterFactory& presenterFactory();
        }

        namespace divideImage
        {
            MEDCOREGUI_EXPORT medAbstractDivideImageProcessPresenterFactory& presenterFactory();
        }
    }

    namespace singleFilterOperation
    {
        namespace addFilter
        {
            MEDCOREGUI_EXPORT medAbstractAddFilterProcessPresenterFactory& presenterFactory();
        }
        namespace divideFilter
        {
            MEDCOREGUI_EXPORT medAbstractDivideFilterProcessPresenterFactory& presenterFactory();
        }
        namespace gaussianFilter
        {
            MEDCOREGUI_EXPORT medAbstractGaussianFilterProcessPresenterFactory& presenterFactory();
        }
        namespace invertFilter
        {
            MEDCOREGUI_EXPORT medAbstractInvertFilterProcessPresenterFactory& presenterFactory();
        }
        namespace medianFilter
        {
            MEDCOREGUI_EXPORT medAbstractMedianFilterProcessPresenterFactory& presenterFactory();
        }
        namespace multiplyFilter
        {
            MEDCOREGUI_EXPORT medAbstractMultiplyFilterProcessPresenterFactory& presenterFactory();
        }
        namespace normalizeFilter
        {
            MEDCOREGUI_EXPORT medAbstractNormalizeFilterProcessPresenterFactory& presenterFactory();
        }
        namespace shrinkFilter
        {
            MEDCOREGUI_EXPORT medAbstractShrinkFilterProcessPresenterFactory& presenterFactory();
        }
        namespace subtractFilter
        {
            MEDCOREGUI_EXPORT medAbstractSubtractFilterProcessPresenterFactory& presenterFactory();
        }
        namespace windowingFilter
        {
            MEDCOREGUI_EXPORT medAbstractWindowingFilterProcessPresenterFactory& presenterFactory();
        }
        namespace imageDenoising
        {
            MEDCOREGUI_EXPORT medAbstractImageDenoisingProcessPresenterFactory& presenterFactory();
        }
        namespace relaxometryEstimation
        {
            MEDCOREGUI_EXPORT medAbstractRelaxometryEstimationProcessPresenterFactory& presenterFactory();
        }
        namespace symmetryAlignment
        {
            MEDCOREGUI_EXPORT medAbstractSymmetryPlaneAlignmentProcessPresenterFactory& presenterFactory();
        }
        namespace biasCorrection
        {
           MEDCOREGUI_EXPORT medAbstractBiasCorrectionProcessPresenterFactory& presenterFactory();
        }
    }

    namespace maskImage
    {
        MEDCOREGUI_EXPORT medAbstractMaskImageProcessPresenterFactory& presenterFactory();
    }

    namespace dwiMasking
    {
        MEDCOREGUI_EXPORT medAbstractDWIMaskingProcessPresenterFactory& presenterFactory();
    }

    namespace diffusionModelEstimation
    {
        MEDCOREGUI_EXPORT medAbstractDiffusionModelEstimationProcessPresenterFactory& presenterFactory();
    }

    namespace diffusionScalarMaps
    {
        MEDCOREGUI_EXPORT medAbstractDiffusionScalarMapsProcessPresenterFactory& presenterFactory();
    }

    namespace tractography
    {
        MEDCOREGUI_EXPORT medAbstractTractographyProcessPresenterFactory& presenterFactory();
    }
}
