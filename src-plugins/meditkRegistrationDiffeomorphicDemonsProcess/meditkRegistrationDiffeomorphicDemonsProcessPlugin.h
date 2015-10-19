#pragma once

#include <dtkCore>
#include <medAbstractRegistrationProcess.h>

class meditkRegistrationDiffeomorphicDemonsProcessPlugin: public medAbstractRegistrationProcessPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractRegistrationProcessPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.meditkRegistrationDiffeomorphicDemonsProcessPlugin" FILE "meditkRegistrationDiffeomorphicDemonsProcessPlugin.json")

public:
     meditkRegistrationDiffeomorphicDemonsProcessPlugin(void) {}
    ~meditkRegistrationDiffeomorphicDemonsProcessPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};

