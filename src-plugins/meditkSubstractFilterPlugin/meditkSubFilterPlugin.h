#pragma once

#include <dtkCore>
#include <medAbstractSubFilter.h>

class meditkSubFilterPlugin: public medAbstractSubFilterPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractSubFilterPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.meditkSubFilterPlugin" FILE "meditkSubFilterPlugin.json")

public:
     meditkSubFilterPlugin(void) {}
    ~meditkSubFilterPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};

