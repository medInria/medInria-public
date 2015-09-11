#pragma once

#include <dtkCore>
#include <medAbstractInvertFilter.h>

class meditkInvertFilterPlugin: public medAbstractInvertFilterPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractInvertFilterPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.meditkInvertFilterPlugin" FILE "meditkInvertFilterPlugin.json")

public:
     meditkInvertFilterPlugin(void) {}
    ~meditkInvertFilterPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};

