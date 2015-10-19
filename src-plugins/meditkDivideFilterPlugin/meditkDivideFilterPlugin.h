#pragma once

#include <dtkCore>
#include <medAbstractDivideFilter.h>

class meditkDivideFilterPlugin: public medAbstractDivideFilterPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractDivideFilterPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.meditkDivideFilterPlugin" FILE "meditkDivideFilterPlugin.json")

public:
     meditkDivideFilterPlugin(void) {}
    ~meditkDivideFilterPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};

