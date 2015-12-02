#pragma once

#include "medArithmeticCommandLineExecutor.h"
#include "medAbstractArithmeticOperationProcess.h"
#include "medAbstractAddImageProcess.h"

class medAddImageCommandLineExecutor : public medArithmeticCommandLineExecutor
{
public:
    medAddImageCommandLineExecutor();

protected:
    virtual medAbstractArithmeticOperationProcess* getProcess(QString key=QString());

};
