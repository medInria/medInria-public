#include <itkTensorScalarMapsProcessPlugin.h>

#include <medCore.h>
#include <medWidgets.h>
#include <itkTensorScalarMapsProcess.h>
#include <itkTensorScalarMapsProcessPresenter.h>

void itkTensorScalarMapsProcessPlugin::initialize(void)
{
    medCore::diffusionScalarMaps::pluginFactory().record(itkTensorScalarMapsProcess::staticMetaObject.className(),
                                                         itkTensorScalarMapsProcessCreator);

    medWidgets::diffusionScalarMaps::presenterFactory().record(itkTensorScalarMapsProcess::staticMetaObject.className(),
                                                               itkTensorScalarMapsProcessPresenterCreator);
}

void itkTensorScalarMapsProcessPlugin::uninitialize(void)
{
}

DTK_DEFINE_PLUGIN(itkTensorScalarMapsProcess)
DTK_DEFINE_PLUGIN(itkTensorScalarMapsProcessPresenter)
