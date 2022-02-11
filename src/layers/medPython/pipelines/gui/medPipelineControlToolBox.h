#pragma once

#include <medToolBox.h>

#include "medPipelineInstance.h"
#include "medPipelineExport.h"

namespace med::pipeline
{

class PipelineControlToolBoxPrivate;

class MEDPIPELINE_EXPORT PipelineControlToolBox : public medToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("medPipelineControlToolBox", "Pipeline toolbox",)

public:
    static bool registered();

    PipelineControlToolBox(QWidget* parent = 0);
    virtual ~PipelineControlToolBox();

    void setPipeline(dtkSmartPointer<PipelineInstance> pipeline);

    bool developerModeEnabled();

    QList<dtkSmartPointer<medAbstractData> > getDroppedResults();

public slots:
    void enableValidation();
    void disableValidation();
    void updateGUI();

private slots:
    void addInitialData();
    void initialDataAdded(QString identifier, dtkSmartPointer<medAbstractData> value, QHash<QString, QVariant> windowLevel);
    void cancelledInitialDataDrop();
    void chooseStep();
    void tryChooseStep();
    void runStep(int step);
    void next();
    void previous();
    void reset();
    void skip();
    void quit();
    void setDeveloperModeEnabled(bool enabled);
    void updateResultDropVisibility();
    void addDroppedResult(const medDataIndex &index);

private:
    PipelineControlToolBoxPrivate* const d;

    bool finalizeStep(bool validate);

    void displayInformativeMessageBox(QString text, QString informativeText);

    bool pipelineHasAllInitialData();
    bool pipelineHasSomeInitialData();

    void clearResultDrops();
    void addResultDropSite();
    void updateResultDropGUI();
};

} // namespace med::pipeline
