#pragma once

#include <dtkCore>
#include <medAbstractAddFilter.h>

class meditkAddFilterPlugin: public medAbstractAddFilterPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractAddFilterPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.meditkAddFilterPlugin" FILE "meditkAddFilterPlugin.json")

public:
     meditkAddFilterPlugin(void) {}
    ~meditkAddFilterPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};

