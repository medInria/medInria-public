#pragma once


#include <QObject>
#include "medCommandLineExecutor.h"
#include "medDataReaderWriter.h"
#include "medAbstractImageData.h"
#include "medIntParameter.h"
#include "medCore.h"

class medArithmeticCommandLineExecutorPrivate;

class medArithmeticCommandLineExecutor:public medCommandLineExecutor
{
public:
    medArithmeticCommandLineExecutor();
    virtual ~medArithmeticCommandLineExecutor();

protected:
    virtual void init();
    virtual void run();
    virtual medAbstractArithmeticOperationProcess* getProcess(QString)=0;

private:
    medArithmeticCommandLineExecutorPrivate* d;
};
