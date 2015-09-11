#pragma once

#include <dtkCore>
#include <medAbstractShrinkFilter.h>

class meditkShrinkFilterPlugin: public medAbstractShrinkFilterPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractShrinkFilterPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.meditkShrinkFilterPlugin" FILE "meditkShrinkFilterPlugin.json")

public:
     meditkShrinkFilterPlugin(void) {}
    ~meditkShrinkFilterPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};

