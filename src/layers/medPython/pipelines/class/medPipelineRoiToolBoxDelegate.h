#pragma once

#include <medDataIndex.h>

#include "medPipelineDelegate.h"
#include "medPipelineStepProperties.h"
#include "medPipelineUtils.h"

namespace med::pipeline
{

class RoiToolboxDelegatePrivate;

class MEDPIPELINE_EXPORT RoiToolboxDelegate : public PipelineDelegate
{
    Q_OBJECT

public:
    static void registerDelegate();

    RoiToolboxDelegate(void* argument = nullptr);
    virtual ~RoiToolboxDelegate();

    static PipelineStepProperties createRoiToolboxStep(QString name);
    void startRoiToolboxStep(dtkSmartPointer<medAbstractData> input);
    void endRoiToolboxStep(dtkSmartPointer<medAbstractData>* output, med::pipeline::OutputType outputType = med::pipeline::MASK);

public slots:
    static PipelineStepProperties createDelegateRoiToolboxStep(QString input, QString output, med::pipeline::OutputType outputType);
    void startRoiToolboxStep(PipelineWorkingState* state);
    void endRoiToolboxStep(PipelineWorkingState* state);

private:
    RoiToolboxDelegatePrivate* const d;

    dtkSmartPointer<medAbstractData> convertToOutputType(dtkSmartPointer<medAbstractData> mask, med::pipeline::OutputType outputType,
                                                         dtkSmartPointer<medAbstractData> sourceImage);
};

} // namespace med::pipeline
