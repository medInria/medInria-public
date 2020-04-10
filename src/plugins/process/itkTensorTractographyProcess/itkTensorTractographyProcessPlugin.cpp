#include <itkTensorTractographyProcessPlugin.h>

#include <medCore.h>
#include <medWidgets.h>
#include <itkTensorTractographyProcess.h>
#include <itkTensorTractographyProcessPresenter.h>

void itkTensorTractographyProcessPlugin::initialize(void)
{
    medCore::tractography::pluginFactory().record(itkTensorTractographyProcess::staticMetaObject.className(),
                                                  itkTensorTractographyProcessCreator);

    medWidgets::tractography::presenterFactory().record(itkTensorTractographyProcess::staticMetaObject.className(),
                                                        itkTensorTractographyProcessPresenterCreator);
}

void itkTensorTractographyProcessPlugin::uninitialize(void)
{
}

DTK_DEFINE_PLUGIN(itkTensorTractographyProcess)
DTK_DEFINE_PLUGIN(itkTensorTractographyProcessPresenter)
