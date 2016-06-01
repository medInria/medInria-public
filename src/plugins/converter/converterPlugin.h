#pragma once

#include <dtkCore>

#include <medAbstractConverter.h>


class converterPlugin: public medAbstractConverterPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractConverterPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.converterPlugin" FILE "converterPlugin.json")

public:
     converterPlugin(void) {}
    ~converterPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};

