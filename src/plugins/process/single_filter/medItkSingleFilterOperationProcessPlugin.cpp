/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkSingleFilterOperationProcessPlugin.h>

#include <medCore.h>
#include <medWidgets.h>
#include <medItkAddProcess.h>
#include <medItkDivideProcess.h>
#include <medItkGaussianProcess.h>
#include <medItkInvertProcess.h>
#include <medItkMedianProcess.h>
#include <medItkMultiplyProcess.h>
#include <medItkNormalizeProcess.h>
#include <medItkShrinkProcess.h>
#include <medItkSubtractProcess.h>
#include <medItkWindowingProcess.h>

#include <medItkAddProcessPresenter.h>
#include <medItkDivideProcessPresenter.h>
#include <medItkGaussianProcessPresenter.h>
#include <medItkInvertProcessPresenter.h>
#include <medItkMedianProcessPresenter.h>
#include <medItkMultiplyProcessPresenter.h>
#include <medItkNormalizeProcessPresenter.h>
#include <medItkShrinkProcessPresenter.h>
#include <medItkSubtractProcessPresenter.h>
#include <medItkWindowingProcessPresenter.h>



void medItkSingleFilterOperationProcessPlugin::initialize(void)
{
    medCore::singleFilterOperation::addFilter::pluginFactory().record(medItkAddProcess::staticMetaObject.className()
                                                                      , medItkAddProcessCreator);
    medCore::singleFilterOperation::divideFilter::pluginFactory().record(medItkDivideProcess::staticMetaObject.className()
                                                                      , medItkDivideProcessCreator);
    medCore::singleFilterOperation::gaussianFilter::pluginFactory().record(medItkGaussianProcess::staticMetaObject.className()
                                                                      , medItkGaussianProcessCreator);
    medCore::singleFilterOperation::invertFilter::pluginFactory().record(medItkInvertProcess::staticMetaObject.className()
                                                                      , medItkInvertProcessCreator);
    medCore::singleFilterOperation::medianFilter::pluginFactory().record(medItkMedianProcess::staticMetaObject.className()
                                                                      , medItkMedianProcessCreator);
    medCore::singleFilterOperation::multiplyFilter::pluginFactory().record(medItkMultiplyProcess::staticMetaObject.className()
                                                                      , medItkMultiplyProcessCreator);
    medCore::singleFilterOperation::normalizeFilter::pluginFactory().record(medItkNormalizeProcess::staticMetaObject.className()
                                                                      , medItkNormalizeProcessCreator);
    medCore::singleFilterOperation::shrinkFilter::pluginFactory().record(medItkShrinkProcess::staticMetaObject.className()
                                                                      , medItkShrinkProcessCreator);
    medCore::singleFilterOperation::subtractFilter::pluginFactory().record(medItkSubtractProcess::staticMetaObject.className()
                                                                      , medItkSubtractProcessCreator);
    medCore::singleFilterOperation::windowingFilter::pluginFactory().record(medItkWindowingProcess::staticMetaObject.className()
                                                                      , medItkWindowingProcessCreator);

    medWidgets::singleFilterOperation::addFilter::presenterFactory().record(medItkAddProcess::staticMetaObject.className()
                                                                      , medItkAddProcessPresenterCreator);
    medWidgets::singleFilterOperation::divideFilter::presenterFactory().record(medItkDivideProcess::staticMetaObject.className()
                                                                      , medItkDivideProcessPresenterCreator);
    medWidgets::singleFilterOperation::gaussianFilter::presenterFactory().record(medItkGaussianProcess::staticMetaObject.className()
                                                                      , medItkGaussianProcessPresenterCreator);
    medWidgets::singleFilterOperation::invertFilter::presenterFactory().record(medItkInvertProcess::staticMetaObject.className()
                                                                      , medItkInvertProcessPresenterCreator);
    medWidgets::singleFilterOperation::medianFilter::presenterFactory().record(medItkMedianProcess::staticMetaObject.className()
                                                                      , medItkMedianProcessPresenterCreator);
    medWidgets::singleFilterOperation::multiplyFilter::presenterFactory().record(medItkMultiplyProcess::staticMetaObject.className()
                                                                      , medItkMultiplyProcessPresenterCreator);
    medWidgets::singleFilterOperation::normalizeFilter::presenterFactory().record(medItkNormalizeProcess::staticMetaObject.className()
                                                                      , medItkNormalizeProcessPresenterCreator);
    medWidgets::singleFilterOperation::shrinkFilter::presenterFactory().record(medItkShrinkProcess::staticMetaObject.className()
                                                                      , medItkShrinkProcessPresenterCreator);
    medWidgets::singleFilterOperation::subtractFilter::presenterFactory().record(medItkSubtractProcess::staticMetaObject.className()
                                                                      , medItkSubtractProcessPresenterCreator);
    medWidgets::singleFilterOperation::windowingFilter::presenterFactory().record(medItkWindowingProcess::staticMetaObject.className()
                                                                      , medItkWindowingProcessPresenterCreator);

}

void medItkSingleFilterOperationProcessPlugin::uninitialize(void)
{

}
DTK_DEFINE_PLUGIN(medItkSingleFilterPlugin)


