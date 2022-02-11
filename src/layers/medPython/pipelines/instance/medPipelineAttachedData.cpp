#include "medPipelineAttachedData.h"

#include <medAbstractDataFactory.h>

namespace med::pipeline
{

class PipelineAttachedDataPrivate
{
public:
    double window, level;
    RenderingParameters renderingParameters;
    bool isFinalResult;
};

bool PipelineAttachedData::registered()
{
    return medAbstractDataFactory::instance()->registerDataType<PipelineAttachedData>();
}

PipelineAttachedData::PipelineAttachedData()
    : d(new PipelineAttachedDataPrivate())
{
    setWindowLevel(-1, -1);
    d->isFinalResult = false;
}

PipelineAttachedData::~PipelineAttachedData()
{
    delete d;
}

void PipelineAttachedData::setWindowLevel(double window, double level)
{
    d->window = window;
    d->level = level;
}

bool PipelineAttachedData::hasWindowLevel()
{
    return (d->window >= 0) && (d->level >= 0);
}

double PipelineAttachedData::getWindow()
{
    return d->window;
}

double PipelineAttachedData::getLevel()
{
    return d->level;
}

void PipelineAttachedData::setFinalResult(bool isFinalResult)
{
    d->isFinalResult = isFinalResult;
}

bool PipelineAttachedData::isFinalResult()
{
    return d->isFinalResult;
}

void PipelineAttachedData::setRenderingParameters(RenderingParameters renderingParameters)
{
    d->renderingParameters = renderingParameters;
    d->isFinalResult = true;
}

bool PipelineAttachedData::hasRenderingParameters()
{
    return d->renderingParameters.renderingMode != "";
}

RenderingParameters PipelineAttachedData::getRenderingParameters()
{
    return d->renderingParameters;
}

} // namespace med::pipeline
