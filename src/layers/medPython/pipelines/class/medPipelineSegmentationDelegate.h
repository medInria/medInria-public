#pragma once

#include <medDataIndex.h>

#include "medPipelineDelegate.h"
#include "medPipelineStepProperties.h"
#include "medPipelineUtils.h"

namespace med::pipeline
{

class SegmentationDelegatePrivate;

class MEDPIPELINE_EXPORT SegmentationDelegate : public PipelineDelegate
{
    Q_OBJECT

public:
    static void registerDelegate();

    SegmentationDelegate(void* argument = nullptr);
    virtual ~SegmentationDelegate();

    static PipelineStepProperties createPaintStep(QString name, QString regionName,
                                                     med::pipeline::RetryMode retryMode = med::pipeline::SINGLE_TRY);
    void startPaintStep(dtkSmartPointer<medAbstractData> input, med::pipeline::RetryMode mode = med::pipeline::SINGLE_TRY);
    void endPaintStep(QList<dtkSmartPointer<medAbstractData> >* outputs, med::pipeline::OutputType outputType = med::pipeline::MASK);

    static PipelineStepProperties createPolygonROIStep(QString name, QString regionName);
    void startPolygonROIStep(dtkSmartPointer<medAbstractData> input);
    void endPolygonROIStep(dtkSmartPointer<medAbstractData>* output, med::pipeline::OutputType outputType = med::pipeline::MASK);

    static PipelineStepProperties createPolygonROISamplingStep(QString name, QString regionName);
    void startPolygonROISamplingStep(dtkSmartPointer<medAbstractData> input);
    void endPolygonROISamplingStep(QList<double>* outputValues);

    static PipelineStepProperties createDraftMagicWandStep(QString name, QString regionName);
    void startDraftMagicWandStep(dtkSmartPointer<medAbstractData> input, QList<double> innerSampleStats, QList<double> outerSampleStats);
    void endDraftMagicWandStep(QList<double>* outputStats);

    static PipelineStepProperties createMagicWandStep(QString name, QString regionName);
    void startMagicWandStep(dtkSmartPointer<medAbstractData> input);
    void startMagicWandStep(dtkSmartPointer<medAbstractData> input, QList<double> innerSampleStats, QList<double> outerSampleStats);
    void startMagicWandStep(dtkSmartPointer<medAbstractData> input, double minThreshold, double maxThreshold);
    void endMagicWandStep(dtkSmartPointer<medAbstractData>* output, med::pipeline::OutputType outputType = med::pipeline::MASK);

    static PipelineStepProperties createMaskPaintCorrectionStep(QString name, QString regionName);
    void startMaskPaintCorrectionStep(dtkSmartPointer<medAbstractData> mask, dtkSmartPointer<medAbstractData> background);
    void endMaskPaintCorrectionStep(dtkSmartPointer<medAbstractData>* output);

    static PipelineStepProperties createVesselStep(QString name, QString vesselName);
    void startVesselStep(dtkSmartPointer<medAbstractData> input, double minRadius, double maxRadius, double power);
    void endVesselStep(dtkSmartPointer<medAbstractData>* output);

    static PipelineStepProperties createHistogramAnalysisStep(QString name);
    void startHistogramAnalysisStep(dtkSmartPointer<medAbstractData> image, dtkSmartPointer<medAbstractData> mask,
                                    med::pipeline::ThresholdType defaultThresholdType = med::pipeline::LOWER_THRESHOLD,
                                    QList<double> vertexPos = QList<double>());
    void endHistogramAnalysisStep(QList<double>* thresholds, med::pipeline::ThresholdType* chosenThresholdType);

    static PipelineStepProperties createVOICutterStep(QString name);
    void startVOICutterStep(dtkSmartPointer<medAbstractData> input);
    void endVOICutterStep(dtkSmartPointer<medAbstractData>* output);

    static PipelineStepProperties createGeodesicActiveContourLevelSetStep(QString name);
    void startGeodesicActiveContourLevelSetStep(dtkSmartPointer<medAbstractData> image, dtkSmartPointer<medAbstractData> mask,
                                                double propagation, double curvature, double advection);
    void endGeodesicActiveContourLevelSetStep(dtkSmartPointer<medAbstractData>* output);
    static PipelineStepProperties createMaskPaintOperationsStep(QString name, QString regionName);
    void startMaskPaintOperationsStep(dtkSmartPointer<medAbstractData> input, dtkSmartPointer<medAbstractData> mask);
    void endMaskPaintOperationsStep(dtkSmartPointer<medAbstractData> *output);

public slots:
    static PipelineStepProperties createDelegatePaintStep(QString regionName, QString input, QString output,
                                                             med::pipeline::OutputType outputType = med::pipeline::MASK,
                                                             med::pipeline::RetryMode retryMode = med::pipeline::SINGLE_TRY);
    void startPaintStep(PipelineWorkingState* state);
    void endPaintStep(PipelineWorkingState* state);

    static PipelineStepProperties createDelegatePolygonROIStep(QString regionName, QString input, QString output,
                                                                  med::pipeline::OutputType outputType = med::pipeline::MASK);
    void startPolygonROIStep(PipelineWorkingState* state);
    void endPolygonROIStep(PipelineWorkingState* state);

    static PipelineStepProperties createDelegatePolygonROISamplingStep(QString regionName, QString input, QString output);
    void startPolygonROISamplingStep(PipelineWorkingState* state);
    void endPolygonROISamplingStep(PipelineWorkingState* state);

    static PipelineStepProperties createDelegateDraftMagicWandStep(QString regionName, QString input, QString innerSampleStats,
                                                                      QString outerSampleStats);
    void startDraftMagicWandStep(PipelineWorkingState* state);
    void endDraftMagicWandStep(PipelineWorkingState* state);

    static PipelineStepProperties createDelegateMagicWandStep(QString regionName, QString input, QString output,
                                                                 med::pipeline::OutputType outputType = med::pipeline::MASK);
    static PipelineStepProperties createDelegateMagicWandStep(QString regionName, QString input, QString innerSampleStats,
                                                                 QString outerSampleStats, QString output,
                                                                 med::pipeline::OutputType outputType = med::pipeline::MASK);
    static PipelineStepProperties createDelegateMagicWandStep(QString regionName, QString input,
                                                                 double minThreshold, double maxThreshold, QString output,
                                                                 med::pipeline::OutputType outputType = med::pipeline::MASK);

    void startMagicWandStep(PipelineWorkingState* state);
    void endMagicWandStep(PipelineWorkingState* state);

    static PipelineStepProperties createDelegateMaskPaintCorrectionStep(QString regionName, QString mask,
                                                                           QString background, QString output);
    void startMaskPaintCorrectionStep(PipelineWorkingState* state);
    void endMaskPaintCorrectionStep(PipelineWorkingState* state);


    static PipelineStepProperties createDelegateVesselStep(QString vesselName, QString input, QString output, QString outputColor,
                                                              double minRadius, double maxRadius, double power);
    void startVesselStep(PipelineWorkingState* state);
    void endVesselStep(PipelineWorkingState* state);

    static PipelineStepProperties createDelegateHistogramAnalysisStep(QString image, QString mask, QString output,
                                                                         med::pipeline::ThresholdType defaultThresholdType = med::pipeline::LOWER_THRESHOLD,
                                                                         QList<double> vertexPos = QList<double>(),
                                                                         int numThresholds = 1);
    void startHistogramAnalysisStep(PipelineWorkingState* state);
    void endHistogramAnalysisStep(PipelineWorkingState* state);

    static PipelineStepProperties createDelegateVOICutterStep(QString input, QString output);
    void startVOICutterStep(PipelineWorkingState* state);
    void endVOICutterStep(PipelineWorkingState* state);

    static PipelineStepProperties createDelegateGeodesicActiveContourLevelSetStep(QString image, QString mask, QString output,
                                                                                     double propagation, double curvature, double advection);
    void startGeodesicActiveContourLevelSetStep(PipelineWorkingState* state);
    void endGeodesicActiveContourLevelSetStep(PipelineWorkingState* state);

    static PipelineStepProperties createDelegateMaskPaintOperationsStep(QString regionName, QString input, QString mask, QString output);
    void startMaskPaintOperationsStep(PipelineWorkingState* state);
    void endMaskPaintOperationsStep(PipelineWorkingState* state);

private:
    static QString asTitle(QString name);

    SegmentationDelegatePrivate* const d;

    dtkSmartPointer<medAbstractData> convertToOutputType(dtkSmartPointer<medAbstractData> mask, med::pipeline::OutputType outputType,
                                                         dtkSmartPointer<medAbstractData> sourceImage);

    dtkSmartPointer<medAbstractData> duplicateMask(dtkSmartPointer<medAbstractData> data);
private slots:
    void retrievePaintedMask();
    void operationsOnPaintedMask(int operationType);
};

} // namespace med::pipeline
