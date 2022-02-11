#pragma once

#include "medPipelineStepProperties.h"
#include "medPipelineTypes.h"
#include "medPipelineWorkingState.h"
#include "medPipelineWorkspace.h"

namespace med::pipeline
{

class MEDPIPELINE_EXPORT PipelineDelegate : public QObject
{
    Q_OBJECT

public:
    static void registerDelegate();

    PipelineDelegate(void* argument = nullptr);
    virtual ~PipelineDelegate();

    virtual QString getClassName();

    virtual void startStep(QString name, PipelineWorkingState* state);
    virtual void endStep(QString name, PipelineWorkingState* state);

protected:
    PipelineWorkspace* getWorkspace();
};

} // namespace med::pipeline
