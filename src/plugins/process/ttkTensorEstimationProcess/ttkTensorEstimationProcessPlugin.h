#pragma once

#include <medAbstractDiffusionModelEstimationProcess.h>
#include <ttkTensorEstimationProcessPluginExport.h>

class TTKTENSORESTIMATIONPROCESSPLUGIN_EXPORT ttkTensorEstimationProcessPlugin : public medAbstractDiffusionModelEstimationProcessPlugin
{
    Q_OBJECT

    Q_INTERFACES(medAbstractDiffusionModelEstimationProcessPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.ttkTensorEstimationProcessPlugin" FILE "ttkTensorEstimationProcessPlugin.json")

public:
     ttkTensorEstimationProcessPlugin() {}
     virtual ~ttkTensorEstimationProcessPlugin() {}

public:
    void initialize();
    void uninitialize();
};
