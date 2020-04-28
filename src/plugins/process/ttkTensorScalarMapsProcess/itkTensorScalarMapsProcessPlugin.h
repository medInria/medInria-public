#pragma once

#include <medAbstractDiffusionScalarMapsProcess.h>
#include <itkTensorScalarMapsProcessPluginExport.h>

class ITKTENSORSCALARMAPSPROCESSPLUGIN_EXPORT itkTensorScalarMapsProcessPlugin : public medAbstractDiffusionScalarMapsProcessPlugin
{
    Q_OBJECT

    Q_INTERFACES(medAbstractDiffusionScalarMapsProcessPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.itkTensorScalarMapsProcessPlugin" FILE "itkTensorScalarMapsProcessPlugin.json")

public:
     itkTensorScalarMapsProcessPlugin() {}
     virtual ~itkTensorScalarMapsProcessPlugin() {}

public:
    void initialize();
    void uninitialize();
};
