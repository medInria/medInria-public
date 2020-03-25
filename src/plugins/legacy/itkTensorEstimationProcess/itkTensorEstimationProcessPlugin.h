#pragma once

#include <medAbstractDiffusionModelEstimationProcess.h>
#include <itkTensorEstimationProcessPluginExport.h>

class ITKTENSORESTIMATIONPROCESSPLUGIN_EXPORT itkTensorEstimationProcessPlugin : public medAbstractDiffusionModelEstimationProcessPlugin
{
    Q_OBJECT

    Q_INTERFACES(medAbstractDiffusionModelEstimationProcessPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.itkTensorEstimationProcessPlugin" FILE "itkTensorEstimationProcessPlugin.json")

public:
     itkTensorEstimationProcessPlugin() {}
     virtual ~itkTensorEstimationProcessPlugin() {}

public:
    void initialize();
    void uninitialize();
};
