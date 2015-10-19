#pragma once

#include <dtkCore>
#include <medAbstractWindowingFilter.h>

class meditkWindowingFilterPlugin: public medAbstractWindowingFilterPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractWindowingFilterPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.meditkWindowingFilterPlugin" FILE "meditkWindowingFilterPlugin.json")

public:
     meditkWindowingFilterPlugin(void) {}
    ~meditkWindowingFilterPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};

