#pragma once

#include "medPipelineFilteringDelegate.h"
#include "medPipelineDelegate.h"
#include "medPipelineStepProperties.h"
#include "medPipelineExport.h"
#include "medPipelineReformatDelegate.h"
#include "medPipelineRegistrationDelegate.h"
#include "medPipelineRoiToolBoxDelegate.h"
#include "medPipelineSegmentationDelegate.h"

namespace med::pipeline
{

class AbstractPipelinePrivate;

class MEDPIPELINE_EXPORT AbstractPipeline : public PipelineDelegate
{
    Q_OBJECT

public:
    AbstractPipeline();
    virtual ~AbstractPipeline();

    virtual QString getTitle() = 0;
    virtual QString getAbbreviation() = 0;

    virtual QHash<QString, QString> getInitialDataIdsAndNames() = 0;

    virtual QList<QString> getMeshOrder() = 0;

    virtual QList<PipelineStepProperties> createSteps() = 0;

    static PipelineStepProperties createDisplayResultsStep(QString name);
    void startDisplayResultsStep(QList<dtkSmartPointer<medAbstractData> > resultData, QHash<QString, QVariant> resultValues);

public slots:
    static PipelineStepProperties createDelegateDisplayResultsStep();
    void startDisplayResultsStep(PipelineWorkingState* state);

protected:
    ReformatDelegate* getReformatDelegate();
    RegistrationDelegate* getRegistrationDelegate();
    SegmentationDelegate* getSegmentationDelegate();
    FilteringDelegate* getFilteringDelegate();
    RoiToolboxDelegate* getRoiToolboxDelegate();

private:
    AbstractPipelinePrivate* const d;
};

} // namespace med::pipeline
