#pragma once

#include <dtkCore>
#include <medAbstractMultFilter.h>

class meditkMultFilterPlugin: public medAbstractMultFilterPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractMultFilterPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.meditkMultFilterPlugin" FILE "meditkMultFilterPlugin.json")

public:
     meditkMultFilterPlugin(void) {}
    ~meditkMultFilterPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};

