#pragma once

#include <medProcessPresenterFactory.h>
#include <medAbstractMorphomathOperationProcessPresenter.h>
#include <medAbstractOpeningImageProcessPresenter.h>
#include <medAbstractMaskImageProcessPresenter.h>

#include <medWidgetsExport.h>

namespace medWidgets
{
    namespace pluginManager
    {
        MEDWIDGETS_EXPORT void initialize(const QString& path = QString(), bool verbose = true);
    }
    namespace morphomathOperation
    {
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
//        namespace closingImage
//        {
//            MEDCORE_EXPORT medAbstractClosingImageProcessPluginFactory& pluginFactory(void);
//        }
    }

    namespace maskImage
    {
        MEDWIDGETS_EXPORT medAbstractMaskImageProcessPresenterFactory& presenterFactory();
    }
}

