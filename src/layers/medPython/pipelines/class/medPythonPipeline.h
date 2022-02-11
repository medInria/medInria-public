#pragma once

#include "medAbstractPipeline.h"
#include "medPython.h"

namespace med::pipeline
{

class MEDPIPELINE_EXPORT PythonPipeline : public AbstractPipeline
{
    Q_OBJECT

public:
    virtual PyObject* getPythonClass() = 0;

    using AbstractPipeline::getFilteringDelegate;
    using AbstractPipeline::getReformatDelegate;
    using AbstractPipeline::getRegistrationDelegate;
    using AbstractPipeline::getRoiToolboxDelegate;
    using AbstractPipeline::getSegmentationDelegate;
    using PipelineDelegate::getWorkspace;
};

} // namespace med::pipeline
