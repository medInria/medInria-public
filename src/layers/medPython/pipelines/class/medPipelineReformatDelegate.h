#pragma once

#include "medPipelineDelegate.h"
#include "medPipelineStepProperties.h"

namespace med::pipeline
{

class ReformatDelegatePrivate;

class MEDPIPELINE_EXPORT ReformatDelegate : public PipelineDelegate
{
    Q_OBJECT

public:
    static void registerDelegate();

    ReformatDelegate(void* argument = nullptr);
    virtual ~ReformatDelegate();

    static PipelineStepProperties createResliceStep(QString name);
    void startResliceStep(dtkSmartPointer<medAbstractData> input, QList<double> spacing = QList<double>());
    void endResliceStep(dtkSmartPointer<medAbstractData>* output);

    static PipelineStepProperties createFrameExtractionStep(QString name);
    void startFrameExtractionStep(dtkSmartPointer<medAbstractData> input);
    void endFrameExtractionStep(dtkSmartPointer<medAbstractData>* output);

    static PipelineStepProperties createCroppingStep(QString name);
    void startCroppingStep(dtkSmartPointer<medAbstractData> input);
    void startCroppingStep(QList<dtkSmartPointer<medAbstractData> > inputs);
    void endCroppingStep(dtkSmartPointer<medAbstractData>* output);
    void endCroppingStep(QList<dtkSmartPointer<medAbstractData> >* outputs);

    static PipelineStepProperties createMergeVolumesStep(QString name);
    void startMergeVolumesStep(dtkSmartPointer<medAbstractData> input);
    void endMergeVolumesStep(dtkSmartPointer<medAbstractData>* output);

public slots:
    static PipelineStepProperties createDelegateResliceStep(QString input, QString output,
                                                               QList<double> spacing = QList<double>());
    void startResliceStep(PipelineWorkingState* state);
    void endResliceStep(PipelineWorkingState* state);

    static PipelineStepProperties createDelegateFrameExtractionStep(QString input, QString output);
    void startFrameExtractionStep(PipelineWorkingState* state);
    void endFrameExtractionStep(PipelineWorkingState* state);

    static PipelineStepProperties createDelegateCroppingStep(QStringList inputs, QStringList outputs);
    void startCroppingStep(PipelineWorkingState* state);
    void endCroppingStep(PipelineWorkingState* state);

    static PipelineStepProperties createDelegateMergeVolumesStep(QString input, QString output);
    void startMergeVolumesStep(PipelineWorkingState* state);
    void endMergeVolumesStep(PipelineWorkingState* state);

private:
    ReformatDelegatePrivate* const d;
};

} // namespace med::pipeline
