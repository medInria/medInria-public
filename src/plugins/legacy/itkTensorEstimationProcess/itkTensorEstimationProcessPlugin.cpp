#include <itkTensorEstimationProcessPlugin.h>

#include <medCore.h>
#include <medWidgets.h>
#include <itkTensorEstimationProcess.h>
#include <itkTensorEstimationProcessPresenter.h>

void itkTensorEstimationProcessPlugin::initialize(void)
{
    medCore::diffusionModelEstimation::pluginFactory().record(itkTensorEstimationProcess::staticMetaObject.className(),
                                                              itkTensorEstimationProcessCreator);

    medWidgets::diffusionModelEstimation::presenterFactory().record(itkTensorEstimationProcess::staticMetaObject.className(),
                                                                    itkTensorEstimationProcessPresenterCreator);
}

void itkTensorEstimationProcessPlugin::uninitialize(void)
{
}

DTK_DEFINE_PLUGIN(itkTensorEstimationProcess)
DTK_DEFINE_PLUGIN(itkTensorEstimationProcessPresenter)
