#pragma once

#include <dtkCore>
#include <medAbstractSubstractFilter.h>

class meditkSubstractFilterPlugin: public medAbstractSubstractFilterPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractSubstractFilterPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.meditkSubstractFilterPlugin" FILE "meditkSubstractFilterPlugin.json")

public:
     meditkSubstractFilterPlugin(void) {}
    ~meditkSubstractFilterPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};

