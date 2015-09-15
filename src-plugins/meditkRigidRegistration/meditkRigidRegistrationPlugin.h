#pragma once

#include <dtkCore>
#include <medAbstractShrinkFilter.h>

class meditkRigidRegistrationPlugin: public medAbstractShrinkFilterPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractShrinkFilterPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.meditkRigidRegistrationPlugin" FILE "meditkRigidRegistrationPlugin.json")

public:
     meditkRigidRegistrationPlugin(void) {}
    ~meditkRigidRegistrationPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};

