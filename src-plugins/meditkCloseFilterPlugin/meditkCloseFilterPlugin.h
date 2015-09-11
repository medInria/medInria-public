#pragma once

#include <dtkCore>
#include <medAbstractCloseFilter.h>

class meditkCloseFilterPlugin: public medAbstractCloseFilterPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractCloseFilterPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.meditkCloseFilterPlugin" FILE "meditkCloseFilterPlugin.json")

public:
     meditkCloseFilterPlugin(void) {}
    ~meditkCloseFilterPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};

