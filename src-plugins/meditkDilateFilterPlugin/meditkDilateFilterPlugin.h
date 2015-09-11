#pragma once

#include <dtkCore>
#include <medAbstractDilateFilter.h>

class meditkDilateFilterPlugin: public medAbstractDilateFilterPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractDilateFilterPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.meditkDilateFilterPlugin" FILE "meditkDilateFilterPlugin.json")

public:
     meditkDilateFilterPlugin(void) {}
    ~meditkDilateFilterPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};

