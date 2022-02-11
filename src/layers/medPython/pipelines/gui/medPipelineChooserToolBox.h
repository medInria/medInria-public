#pragma once

#include <dtkCoreSupport/dtkSmartPointer>

#include <medToolBox.h>

#include "medPipelineExport.h"

namespace med::pipeline
{

class PipelineChooserToolBoxPrivate;
class PipelineInstance;

class MEDPIPELINE_EXPORT PipelineChooserToolBox : public medToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Pipelines", "Pipeline chooser tool box",)

public:
    static bool registered();

    PipelineChooserToolBox(QWidget* parent = 0);
    virtual ~PipelineChooserToolBox();

    void createDropContainer();

public slots:
    void startNewPipeline(QString identifier);
    void continueExistingPipeline(const medDataIndex& index);

private slots:
    void dataDropped();

private:
    PipelineChooserToolBoxPrivate* const d;

    void createLaunchButtonsForPipelines();
    void runPipeline(dtkSmartPointer<PipelineInstance> pipeline);
};

} // namespace med::pipeline
