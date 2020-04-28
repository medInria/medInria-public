#include "ttkTensorTractographyProcessPlugin.h"

#include <medCore.h>
#include <medWidgets.h>
#include "ttkTensorTractographyProcess.h"
#include "ttkTensorTractographyProcessPresenter.h"

void ttkTensorTractographyProcessPlugin::initialize(void)
{
    medCore::tractography::pluginFactory().record(ttkTensorTractographyProcess::staticMetaObject.className(), ttkTensorTractographyProcessCreator);
    medWidgets::tractography::presenterFactory().record(itkTensorTractographyProcess::staticMetaObject.className(), itkTensorTractographyProcessPresenterCreator);
}

void itkTensorTractographyProcessPlugin::uninitialize(void)
{
}

DTK_DEFINE_PLUGIN(itkTensorTractographyProcess)
DTK_DEFINE_PLUGIN(itkTensorTractographyProcessPresenter)
