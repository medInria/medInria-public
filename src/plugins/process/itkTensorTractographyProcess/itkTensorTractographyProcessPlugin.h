#pragma once

#include <medAbstractTractographyProcess.h>
#include <itkTensorTractographyProcessPluginExport.h>

class ITKTENSORTRACTOGRAPHYPROCESSPLUGIN_EXPORT itkTensorTractographyProcessPlugin : public medAbstractTractographyProcessPlugin
{
    Q_OBJECT

    Q_INTERFACES(medAbstractTractographyProcessPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.itkTensorTractographyProcessPlugin" FILE "itkTensorTractographyProcessPlugin.json")

public:
     itkTensorTractographyProcessPlugin() {}
     virtual ~itkTensorTractographyProcessPlugin() {}

public:
    void initialize();
    void uninitialize();
};
