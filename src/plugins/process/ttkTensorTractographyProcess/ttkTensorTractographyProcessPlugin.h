#pragma once

#include <medAbstractTractographyProcess.h>
#include "ttkTensorTractographyProcessPluginExport.h"

class TTKTENSORTRACTOGRAPHYPROCESSPLUGIN_EXPORT ttkTensorTractographyProcessPlugin : public medAbstractTractographyProcessPlugin
{
    Q_OBJECT

    Q_INTERFACES(medAbstractTractographyProcessPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.ttkTensorTractographyProcessPlugin" FILE "ttkTensorTractographyProcessPlugin.json")

public:
     ttkTensorTractographyProcessPlugin() {}
     virtual ~ttkTensorTractographyProcessPlugin() {}

public:
    void initialize();
    void uninitialize();
};
