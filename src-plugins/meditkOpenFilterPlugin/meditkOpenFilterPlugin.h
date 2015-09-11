#pragma once

#include <dtkCore>
#include <medAbstractOpenFilter.h>

class meditkOpenFilterPlugin: public medAbstractOpenFilterPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractOpenFilterPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.meditkOpenFilterPlugin" FILE "meditkOpenFilterPlugin.json")

public:
     meditkOpenFilterPlugin(void) {}
    ~meditkOpenFilterPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};

