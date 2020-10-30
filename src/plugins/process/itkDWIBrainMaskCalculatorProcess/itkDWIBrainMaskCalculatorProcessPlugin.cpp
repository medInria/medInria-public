#include <itkDWIBrainMaskCalculatorProcessPlugin.h>

#include <medCore.h>
#include <medWidgets.h>
#include <itkDWIBrainMaskCalculatorProcess.h>
#include <itkDWIBrainMaskCalculatorProcessPresenter.h>

void itkDWIBrainMaskCalculatorProcessPlugin::initialize(void)
{
    medCore::dwiMasking::pluginFactory().record(itkDWIBrainMaskCalculatorProcess::staticMetaObject.className(),
                                                itkDWIBrainMaskCalculatorProcessCreator);

    medWidgets::dwiMasking::presenterFactory().record(itkDWIBrainMaskCalculatorProcess::staticMetaObject.className(),
                                                      itkDWIBrainMaskCalculatorProcessPresenterCreator);
}

void itkDWIBrainMaskCalculatorProcessPlugin::uninitialize(void)
{
}

DTK_DEFINE_PLUGIN(itkDWIBrainMaskCalculatorProcess)
DTK_DEFINE_PLUGIN(itkDWIBrainMaskCalculatorProcessPresenter)
