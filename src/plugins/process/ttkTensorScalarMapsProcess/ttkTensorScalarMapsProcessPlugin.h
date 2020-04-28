#pragma once

#include <medAbstractDiffusionScalarMapsProcess.h>
#include <ttkTensorScalarMapsProcessPluginExport.h>

class TTKTENSORSCALARMAPSPROCESSPLUGIN_EXPORT ttkTensorScalarMapsProcessPlugin : public medAbstractDiffusionScalarMapsProcessPlugin
{
    Q_OBJECT

    Q_INTERFACES(medAbstractDiffusionScalarMapsProcessPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.ttkTensorScalarMapsProcessPlugin" FILE "ttkTensorScalarMapsProcessPlugin.json")

public:
     ttkTensorScalarMapsProcessPlugin() {}
     virtual ~ttkTensorScalarMapsProcessPlugin() {}

public:
    void initialize();
    void uninitialize();
};
