#pragma once

/* This class represents instances of a Python pipeline. When an instance is created, the source code
 * of the corresponding pipeline is loaded and stored in the instance so that it can be reloaded later.
 * When reloading an instance, this code is written to a temporary module file (see
 * PythonModuleManager). The name of the module file is the name of the module and is the
 * value returned by getModuleName. The initial module that was used when creating the pipeline is
 * the reference module (returned by getReferenceModuleName). This name is used as a base to generate
 * the temporary name, and also when the reference module needs to be loaded for developer mode.
 */

#include "medPipelineInstance.h"
#include "medPythonPipeline.h"

namespace med::pipeline
{

class PythonPipelineInstancePrivate;

class MEDPIPELINE_EXPORT PythonPipelineInstance : public PipelineInstance
{
    Q_OBJECT
    MED_DATA_INTERFACE("Python pipeline instance", "Python pipeline instance")

public:
    static bool registered();

    PythonPipelineInstance();
    PythonPipelineInstance(PythonPipeline* pipeline);
    PythonPipelineInstance(PythonPipeline* pipeline, QString referenceModuleName, QString sourceCode);
    PythonPipelineInstance(const PythonPipelineInstance& other);
    virtual ~PythonPipelineInstance();

    QString getReferenceModuleName();
    QString getModuleName();
    QString getSourceCode();

protected:
    virtual void createDelegateIfNeeded();

private:
    PythonPipelineInstancePrivate* const d;

    void retrieveModuleNameAndSourceCode();
};

} // namespace med::pipeline

Q_DECLARE_METATYPE(med::pipeline::PythonPipelineInstance)
