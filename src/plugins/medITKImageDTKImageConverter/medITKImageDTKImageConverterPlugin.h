#pragma once

#include <dtkCore>

#include <medAbstractDataConverter.h>


class medITKImageDTKImageConverterPlugin: public medAbstractDataConverterPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractDataConverterPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.medITKImageDTKImageConverterPlugin" FILE "medITKImageDTKImageConverterPlugin.json")

public:
     medITKImageDTKImageConverterPlugin(void) {}
    ~medITKImageDTKImageConverterPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};

