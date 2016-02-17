#pragma once

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

#include <medWidgetsExport.h>

namespace medWidgets
{
    namespace pluginManager
    {
        MEDWIDGETS_EXPORT void initialize(const QString& path = QString(), bool verbose = true);
    }

    namespace generic
    {
        MEDWIDGETS_EXPORT medAbstractProcessPresenterFactory& presenterFactory();
    }

    namespace morphomathOperation
    {
        namespace erodeImage
        {
            MEDWIDGETS_EXPORT medAbstractErodeImageProcessPresenterFactory& presenterFactory();
        }
        namespace dilateImage
        {
            MEDWIDGETS_EXPORT medAbstractDilateImageProcessPresenterFactory& presenterFactory();
        }
        namespace openingImage
        {
            MEDWIDGETS_EXPORT medAbstractOpeningImageProcessPresenterFactory& presenterFactory();
        }
        namespace closingImage
        {
            MEDWIDGETS_EXPORT medAbstractClosingImageProcessPresenterFactory& presenterFactory();
        }
    }

    namespace arithmeticOperation
    {
        namespace addImage
        {
            MEDWIDGETS_EXPORT medAbstractAddImageProcessPresenterFactory& presenterFactory();
        }

        namespace subtractImage
        {
            MEDWIDGETS_EXPORT medAbstractSubtractImageProcessPresenterFactory& presenterFactory();
        }

        namespace multiplyImage
        {
            MEDWIDGETS_EXPORT medAbstractMultiplyImageProcessPresenterFactory& presenterFactory();
        }

        namespace divideImage
        {
            MEDWIDGETS_EXPORT medAbstractDivideImageProcessPresenterFactory& presenterFactory();
        }
    }


    namespace singleFilterOperation
    {
        namespace addFilter
        {
            MEDWIDGETS_EXPORT medAbstractAddFilterProcessPresenterFactory& presenterFactory();
        }
        namespace divideFilter
        {
            MEDWIDGETS_EXPORT medAbstractDivideFilterProcessPresenterFactory& presenterFactory();
        }
        namespace gaussianFilter
        {
            MEDWIDGETS_EXPORT medAbstractGaussianFilterProcessPresenterFactory& presenterFactory();
        }
        namespace invertFilter
        {
            MEDWIDGETS_EXPORT medAbstractInvertFilterProcessPresenterFactory& presenterFactory();
        }
        namespace medianFilter
        {
            MEDWIDGETS_EXPORT medAbstractMedianFilterProcessPresenterFactory& presenterFactory();
        }
        namespace multiplyFilter
        {
            MEDWIDGETS_EXPORT medAbstractMultiplyFilterProcessPresenterFactory& presenterFactory();
        }
        namespace normalizeFilter
        {
            MEDWIDGETS_EXPORT medAbstractNormalizeFilterProcessPresenterFactory& presenterFactory();
        }
        namespace shrinkFilter
        {
            MEDWIDGETS_EXPORT medAbstractShrinkFilterProcessPresenterFactory& presenterFactory();
        }
        namespace subtractFilter
        {
            MEDWIDGETS_EXPORT medAbstractSubtractFilterProcessPresenterFactory& presenterFactory();
        }
        namespace windowingFilter
        {
            MEDWIDGETS_EXPORT medAbstractWindowingFilterProcessPresenterFactory& presenterFactory();
        }
        namespace imageDenoising
        {
            MEDWIDGETS_EXPORT medAbstractImageDenoisingProcessPresenterFactory& presenterFactory();
        }
    }

    namespace maskImage
    {
        MEDWIDGETS_EXPORT medAbstractMaskImageProcessPresenterFactory& presenterFactory();
    }

    namespace dwiMasking
    {
        MEDWIDGETS_EXPORT medAbstractDWIMaskingProcessPresenterFactory& presenterFactory();
    }

    namespace diffusionModelEstimation
    {
        MEDWIDGETS_EXPORT medAbstractDiffusionModelEstimationProcessPresenterFactory& presenterFactory();
    }

    namespace diffusionScalarMaps
    {
        MEDWIDGETS_EXPORT medAbstractDiffusionScalarMapsProcessPresenterFactory& presenterFactory();
    }

    namespace tractography
    {
        MEDWIDGETS_EXPORT medAbstractTractographyProcessPresenterFactory& presenterFactory();
    }
}

