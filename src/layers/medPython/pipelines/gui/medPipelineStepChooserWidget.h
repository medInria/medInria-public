#pragma once

#include <QWidget>

#include "medPipelineInstance.h"
#include "medPipelineExport.h"

namespace med::pipeline
{

class PipelineStepChooserWidgetPrivate;

class MEDPIPELINE_EXPORT PipelineStepChooserWidget : public QWidget
{
    Q_OBJECT

signals:
    void stepChosen(int step);
    void quit();

public:
    PipelineStepChooserWidget(QWidget* parent = 0);
    virtual ~PipelineStepChooserWidget();

    void setPipeline(dtkSmartPointer<PipelineInstance> pipeline);

public slots:
    void refresh();

private:
    PipelineStepChooserWidgetPrivate* const d;

    void clearWidget();
};

} // namespace med::pipeline
