#pragma once

#include <medProcessPresenterFactory.h>
#include <medAbstractMorphomathOperationProcessPresenter.h>
#include <medAbstractArithmeticOperationProcessPresenter.h>
#include <medAbstractOpeningImageProcessPresenter.h>
#include <medAbstractClosingImageProcessPresenter.h>
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
        MEDWIDGETS_EXPORT medAbstractMorphomathOperationProcessPresenterFactory& presenterFactory();
//        MEDWIDGETS_EXPORT medAbstractMorphomathOperationProcessPluginManager& pluginManager(void);
//        MEDWIDGETS_EXPORT void initialize(const QString& path, bool verbose = true);

//        namespace erodeImage
//        {
//            MEDCORE_EXPORT medAbstractErodeImageProcessPluginFactory& pluginFactory(void);
//        }
//        namespace dilateImage
//        {
//            MEDCORE_EXPORT medAbstractDilateImageProcessPluginFactory& pluginFactory(void);
//        }
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
        MEDWIDGETS_EXPORT medAbstractArithmeticOperationProcessPresenterFactory& presenterFactory();
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

