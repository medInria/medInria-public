#pragma once

#include <dtkCore>
#include <medAbstractMedianFilter.h>

class meditkMedianFilterPlugin: public medAbstractMedianFilterPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractMedianFilterPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.meditkMedianFilterPlugin" FILE "meditkMedianFilterPlugin.json")

public:
     meditkMedianFilterPlugin(void) {}
    ~meditkMedianFilterPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};

