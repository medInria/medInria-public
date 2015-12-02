#pragma once


#include "medArithmeticCommandLineExecutor.h"
#include "medAbstractArithmeticOperationProcess.h"
#include "medAbstractOpeningImageProcess.h"

class medSubstractImageCommandLineExecutor : public medArithmeticCommandLineExecutor
{
public:
    medSubstractImageCommandLineExecutor();

protected:
    virtual medAbstractArithmeticOperationProcess* getProcess(QString key=QString());

};
