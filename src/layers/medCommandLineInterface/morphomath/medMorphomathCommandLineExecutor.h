#pragma once


#include <QObject>
#include "medCommandLineExecutor.h"
#include "medDataReaderWriter.h"
#include "medAbstractImageData.h"
#include "medIntParameter.h"
#include "medCore.h"

class medMorphomathCommandLineExecutorPrivate;

class medMorphomathCommandLineExecutor:public medCommandLineExecutor
{
public:
    medMorphomathCommandLineExecutor();
    virtual ~medMorphomathCommandLineExecutor();

protected:
    virtual void init();
    virtual void run();
    virtual medAbstractMorphomathOperationProcess* getProcess(QString)=0;

private:
    medMorphomathCommandLineExecutorPrivate* d;
};
