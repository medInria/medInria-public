#pragma once

#include "medMorphomathCommandLineExecutor.h"
#include "medAbstractMorphomathOperationProcess.h"
#include "medAbstractOpeningImageProcess.h"

class medErodeOperationCommandLineExecutor : public medMorphomathCommandLineExecutor
{
public:
    medErodeOperationCommandLineExecutor();

protected:
    virtual medAbstractMorphomathOperationProcess* getProcess(QString key=QString());

};
