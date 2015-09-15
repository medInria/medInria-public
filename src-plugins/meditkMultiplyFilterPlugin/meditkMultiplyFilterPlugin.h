#pragma once

#include <dtkCore>
#include <medAbstractMultiplyFilter.h>

class meditkMultiplyFilterPlugin: public medAbstractMultiplyFilterPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractMultiplyFilterPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.meditkMultiplyFilterPlugin" FILE "meditkMultiplyFilterPlugin.json")

public:
     meditkMultiplyFilterPlugin(void) {}
    ~meditkMultiplyFilterPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};

