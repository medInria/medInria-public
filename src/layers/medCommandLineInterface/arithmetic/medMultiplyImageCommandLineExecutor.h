#pragma once

#include "medArithmeticCommandLineExecutor.h"
#include "medAbstractArithmeticOperationProcess.h"
#include "medAbstractOpeningImageProcess.h"

class medMultiplyImageCommandLineExecutor : public medArithmeticCommandLineExecutor
{
public:
    medMultiplyImageCommandLineExecutor();

protected:
    virtual medAbstractArithmeticOperationProcess* getProcess(QString key=QString());

};
