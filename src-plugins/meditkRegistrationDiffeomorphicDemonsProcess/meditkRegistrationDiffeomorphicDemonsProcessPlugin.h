#pragma once

#include <dtkCore>
#include <medAbstractRegistrationDiffeomorphicDemonsProcess.h>

class meditkRegistrationDiffeomorphicDemonsProcessPlugin: public medAbstractRegistrationDiffeomorphicDemonsProcessPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractRegistrationDiffeomorphicDemonsProcessPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.meditkRegistrationDiffeomorphicDemonsProcessPlugin" FILE "meditkRegistrationDiffeomorphicDemonsProcessPlugin.json")

public:
     meditkRegistrationDiffeomorphicDemonsProcessPlugin(void) {}
    ~meditkRegistrationDiffeomorphicDemonsProcessPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};

