#pragma once

#include <medAbstractTractographyProcess.h>
#include "ttkTensorTractographyProcessPluginExport.h"

class TTKTENSORTRACTOGRAPHYPROCESSPLUGIN_EXPORT itkTensorTractographyProcessPlugin : public medAbstractTractographyProcessPlugin
{
    Q_OBJECT

    Q_INTERFACES(medAbstractTractographyProcessPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.ttkTensorTractographyProcessPlugin" FILE "ttkTensorTractographyProcessPlugin.json")

public:
     itkTensorTractographyProcessPlugin() {}
     virtual ~itkTensorTractographyProcessPlugin() {}

public:
    void initialize();
    void uninitialize();
};
