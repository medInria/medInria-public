#pragma once

#include <medAttachedData.h>

#include "medPipelineTypes.h"
#include "medPipelineExport.h"

namespace med::pipeline
{

class PipelineAttachedDataPrivate;

class MEDPIPELINE_EXPORT PipelineAttachedData : public medAttachedData
{
    Q_OBJECT
    MED_DATA_INTERFACE("medPipelineAttachedData", "Pipeline related attached data")

public:
    static bool registered();

    PipelineAttachedData();
    virtual ~PipelineAttachedData();

    void setWindowLevel(double window, double level);
    bool hasWindowLevel();
    double getWindow();
    double getLevel();

    void setRenderingParameters(med::pipeline::RenderingParameters renderingParameters);
    bool hasRenderingParameters();
    med::pipeline::RenderingParameters getRenderingParameters();

    void setFinalResult(bool isFinalResult);
    bool isFinalResult();

private:
    PipelineAttachedDataPrivate* const d;
};

} // namespace med::pipeline

Q_DECLARE_METATYPE(med::pipeline::PipelineAttachedData)
