#pragma once

#include <medAbstractArithmeticOperationProcess.h>


class medItkSubstractImageProcessPlugin : public medAbstractSubstractimageProcessPlugin
{
    Q_OBJECT
    Q_INTERFACES(medAbstractSubstractimageProcessPlugin)
    Q_PLUGIN_METADATA(IID "fr.inria.medItkSubstractImageProcessPlugin" FILE "medItkSubstractImageProcessPlugin.json")

public:
     medItkSubstractImageProcessPlugin(void) {}
    virtual ~medItkSubstractImageProcessPlugin(void) {}

public:
    void initialize(void);
    void uninitialize(void);
};
