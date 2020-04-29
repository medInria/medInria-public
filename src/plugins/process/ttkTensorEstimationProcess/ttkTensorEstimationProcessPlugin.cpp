#include "ttkTensorEstimationProcessPlugin.h"

#include <medCore.h>
#include <medWidgets.h>
#include <ttkTensorEstimationProcess.h>
#include <ttkTensorEstimationProcessPresenter.h>

void ttkTensorEstimationProcessPlugin::initialize(void)
{
    medCore::diffusionModelEstimation::pluginFactory().record(ttkTensorEstimationProcess::staticMetaObject.className(),
                                                              ttkTensorEstimationProcessCreator);

    medWidgets::diffusionModelEstimation::presenterFactory().record(ttkTensorEstimationProcess::staticMetaObject.className(),
                                                                    ttkTensorEstimationProcessPresenterCreator);
}

void ttkTensorEstimationProcessPlugin::uninitialize(void)
{
}

DTK_DEFINE_PLUGIN(ttkTensorEstimationProcess)
DTK_DEFINE_PLUGIN(ttkTensorEstimationProcessPresenter)
