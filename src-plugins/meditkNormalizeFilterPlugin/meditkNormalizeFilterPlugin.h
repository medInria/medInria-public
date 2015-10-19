#pragma once

#include <dtkCore>
#include <medAbstractNormalizeFilter.h>

class meditkNormalizeFilterPlugin: public medAbstractNormalizeFilterPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractNormalizeFilterPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.meditkNormalizeFilterPlugin" FILE "meditkNormalizeFilterPlugin.json")

public:
     meditkNormalizeFilterPlugin(void) {}
    ~meditkNormalizeFilterPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};

