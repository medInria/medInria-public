#pragma once


#include "medMorphomathCommandLineExecutor.h"
#include "medAbstractMorphomathOperationProcess.h"
#include "medAbstractOpeningImageProcess.h"

class medOpeningOperationCommandLineExecutor : public medMorphomathCommandLineExecutor
{
public:
    medOpeningOperationCommandLineExecutor();

protected:
    virtual medAbstractMorphomathOperationProcess* getProcess(QString key=QString());

};
