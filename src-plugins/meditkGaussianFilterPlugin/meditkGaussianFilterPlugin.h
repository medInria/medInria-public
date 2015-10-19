#pragma once

#include <dtkCore>
#include <medAbstractGaussianFilter.h>

class meditkGaussianFilterPlugin: public medAbstractGaussianFilterPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractGaussianFilterPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.meditkGaussianFilterPlugin" FILE "meditkGaussianFilterPlugin.json")

public:
     meditkGaussianFilterPlugin(void) {}
    ~meditkGaussianFilterPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};

