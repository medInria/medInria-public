#pragma once

#include "medPipelineDelegate.h"
#include "medPipelineStepProperties.h"

namespace med::pipeline
{

class RegistrationDelegatePrivate;

class MEDPIPELINE_EXPORT RegistrationDelegate : public PipelineDelegate
{
    Q_OBJECT

public:
    static void registerDelegate();

    RegistrationDelegate(void* argument = nullptr);
    virtual ~RegistrationDelegate();

    static PipelineStepProperties createManualRegistrationStep(QString name);
    void startManualRegistrationStep(dtkSmartPointer<medAbstractData> fixed, dtkSmartPointer<medAbstractData> moving);
    void endManualRegistrationStep(dtkSmartPointer<medAbstractData>* output);

public slots:
    static PipelineStepProperties createDelegateManualRegistrationStep(QString fixed, QString moving, QString output);
    void startManualRegistrationStep(PipelineWorkingState* state);
    void endManualRegistrationStep(PipelineWorkingState* state);

private:
    RegistrationDelegatePrivate* const d;
};

} // namespace med::pipeline
