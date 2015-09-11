#pragma once

#include <dtkCore>
#include <medAbstractErodeFilter.h>

class meditkErodeFilterPlugin: public medAbstractErodeFilterPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractErodeFilterPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.meditkErodeFilterPlugin" FILE "meditkErodeFilterPlugin.json")

public:
     meditkErodeFilterPlugin(void) {}
    ~meditkErodeFilterPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};

