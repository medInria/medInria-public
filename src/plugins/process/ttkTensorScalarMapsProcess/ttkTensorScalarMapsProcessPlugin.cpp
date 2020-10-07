#include "ttkTensorScalarMapsProcessPlugin.h"

#include <medCore.h>
#include <medWidgets.h>
#include "ttkTensorScalarMapsProcess.h"
#include "ttkTensorScalarMapsProcessPresenter.h"

void ttkTensorScalarMapsProcessPlugin::initialize(void)
{
    medCore::diffusionScalarMaps::pluginFactory().record(ttkTensorScalarMapsProcess::staticMetaObject.className(),
                                                         ttkTensorScalarMapsProcessCreator);

    medWidgets::diffusionScalarMaps::presenterFactory().record(ttkTensorScalarMapsProcess::staticMetaObject.className(),
                                                               ttkTensorScalarMapsProcessPresenterCreator);
}

void ttkTensorScalarMapsProcessPlugin::uninitialize(void)
{
}

DTK_DEFINE_PLUGIN(ttkTensorScalarMapsProcess)
DTK_DEFINE_PLUGIN(ttkTensorScalarMapsProcessPresenter)
