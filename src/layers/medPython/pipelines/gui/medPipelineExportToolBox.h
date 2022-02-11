#pragma once

#include <medAbstractLayeredView.h>
#include <medToolBox.h>

#include "medPipelineExport.h"

namespace med::pipeline
{

class PipelineExportToolBoxPrivate;

class MEDPIPELINE_EXPORT PipelineExportToolBox : public medToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Export", "Pipeline export toolbox",)

public:
    static bool registered();

    PipelineExportToolBox(QWidget* parent = 0);
    virtual ~PipelineExportToolBox();

public slots:
    void exportToNavX();
    void exportToCarto();
    void exportScene();

protected:
    QList<QString> getSeriesDescriptions(medAbstractLayeredView *view);
    void removeStepNumbersInSeriesDescription(medAbstractLayeredView* view);
    void setSeriesDescriptions(medAbstractLayeredView *view, QList<QString> descriptions);

private slots:
    void stopWaiting();

private:
    PipelineExportToolBoxPrivate* const d;
};

} // namespace med::pipeline
