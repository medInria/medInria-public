#pragma once

#include "medArithmeticCommandLineExecutor.h"
#include "medAbstractArithmeticOperationProcess.h"
#include "medAbstractDivideImageProcess.h"

class medDivideImageCommandLineExecutor : public medArithmeticCommandLineExecutor
{
protected:
    virtual medAbstractArithmeticOperationProcess* getProcess(QString key=QString());

};
