#pragma once

#include <medAbstractDWIMaskingProcess.h>

#include <itkDWIBrainMaskCalculatorProcessPluginExport.h>

class ITKDWIBRAINMASKCALCULATORPROCESSPLUGIN_EXPORT itkDWIBrainMaskCalculatorProcessPlugin : public medAbstractDWIMaskingProcessPlugin
{
    Q_OBJECT

    Q_INTERFACES(medAbstractDWIMaskingProcessPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.itkDWIBrainMaskCalculatorProcessPlugin" FILE "itkDWIBrainMaskCalculatorProcessPlugin.json")

public:
     itkDWIBrainMaskCalculatorProcessPlugin() {}
     virtual ~itkDWIBrainMaskCalculatorProcessPlugin() {}

public:
    void initialize();
    void uninitialize();
};
