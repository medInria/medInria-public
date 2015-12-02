#pragma once

#include "medMorphomathCommandLineExecutor.h"
#include "medAbstractMorphomathOperationProcess.h"
#include "medAbstractOpeningImageProcess.h"

class medDilateOperationCommandLineExecutor : public medMorphomathCommandLineExecutor
{
public:
    medDilateOperationCommandLineExecutor();

protected:
    virtual medAbstractMorphomathOperationProcess* getProcess(QString key=QString());

};
