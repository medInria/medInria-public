#pragma once

#include <dtkCore>
#include <medAbstractSegmentationProcess.h>

class medManualSegmentationPlugin: public medAbstractSegmentationProcessPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractSegmentationProcessPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.medManualSegmentationPlugin" FILE "medManualSegmentationPlugin.json")

public:
     medManualSegmentationPlugin(void) {}
    ~medManualSegmentationPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};

