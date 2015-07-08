#pragma once

#include <medAbstractArithmeticOperationProcess.h>

class medItkSubstractImageProcess: public medAbstractSubstractimageProcess
{
public:
    medItkSubstractImageProcess(QObject* parent = NULL);
    ~medItkSubstractImageProcess();
    virtual void start();
};
