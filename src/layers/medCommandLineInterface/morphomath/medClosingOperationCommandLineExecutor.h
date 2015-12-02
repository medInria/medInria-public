#pragma once

#include "medMorphomathCommandLineExecutor.h"
#include "medAbstractMorphomathOperationProcess.h"
#include "medAbstractOpeningImageProcess.h"

class medClosingOperationCommandLineExecutor : public medMorphomathCommandLineExecutor
{
public:
    medClosingOperationCommandLineExecutor();

protected:
    virtual medAbstractMorphomathOperationProcess* getProcess(QString key=QString());

};
