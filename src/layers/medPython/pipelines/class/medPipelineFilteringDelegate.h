#pragma once

#include "medPipelineConstants.h"
#include "medPipelineDelegate.h"
#include "medPipelineStepProperties.h"

namespace med::pipeline
{

class FilteringDelegatePrivate;

class MEDPIPELINE_EXPORT FilteringDelegate : public PipelineDelegate
{
    Q_OBJECT

public:
    static void registerDelegate();

    FilteringDelegate(void* argument = nullptr);
    virtual ~FilteringDelegate();

    static PipelineStepProperties createSizeThresholdingStep(QString name);
    void startSizeThresholdingStep(dtkSmartPointer<medAbstractData> input,
                                   med::pipeline::RetryMode retryMode = med::pipeline::MULTIPLE_TRIES);
    void endSizeThresholdingStep(dtkSmartPointer<medAbstractData>* output, double* chosenThreshold = nullptr);

    static PipelineStepProperties createThresholdingStep(QString name);
    void startThresholdingStep(dtkSmartPointer<medAbstractData> input,
                               double thresholdValue, int outsideValue,
                               med::pipeline::ThresholdType thresholdType, bool showHistogram = false,
                               double lowerValue = 0., double upperValue = 0.);
    void endThresholdingStep(dtkSmartPointer<medAbstractData>* output);

    static PipelineStepProperties createBinaryThresholdingStep(QString name);
    void startBinaryThresholdingStep(dtkSmartPointer<medAbstractData> input,
                                     int insideValue, int outsideValue,
                                     bool showHistogram = false,
                                     double lowerValue = 0., double upperValue = 0.);
    void endBinaryThresholdingStep(dtkSmartPointer<medAbstractData>* output);

    static PipelineStepProperties createMorphologicalFilteringStep(QString name, med::pipeline::MorphologicalFilterType filterType);
    void startMorphologicalFilteringStep(dtkSmartPointer<medAbstractData> input, med::pipeline::MorphologicalFilterType filterType,
                                         med::pipeline::RetryMode retryMode = med::pipeline::MULTIPLE_TRIES,
                                         med::pipeline::RadiusType radiusType = med::pipeline::RADIUS_IN_MM_OR_PIXELS,
                                         int initialRadius = -1);
    void endMorphologicalFilteringStep(dtkSmartPointer<medAbstractData>* output, int* chosenRadius = nullptr,
                                       med::pipeline::RadiusType* chosenRadiusType = nullptr);

public slots:
    static PipelineStepProperties createDelegateSizeThresholdingStep(QString input, QString output, QString thresholdOutput,
                                                                        med::pipeline::RetryMode retryMode = med::pipeline::MULTIPLE_TRIES);
    void startSizeThresholdingStep(PipelineWorkingState* state);
    void endSizeThresholdingStep(PipelineWorkingState* state);

    static PipelineStepProperties createDelegateThresholdingStep(QString input, QString output,
                                                                    double threshold, int outsideValue, med::pipeline::ThresholdType thresholdType,
                                                                    bool showHistogram = false,
                                                                    double lowerValue = 0., double upperValue= 0.);
    void startThresholdingStep(PipelineWorkingState* state);
    void endThresholdingStep(PipelineWorkingState* state);

    static PipelineStepProperties createDelegateBinaryThresholdingStep(QString input, QString output,
                                                                          int insideValue, int outsideValue, bool showHistogram = false,
                                                                          double lowerValue = 0., double upperValue= 0.);
    void startBinaryThresholdingStep(PipelineWorkingState* state);
    void endBinaryThresholdingStep(PipelineWorkingState* state);

    static PipelineStepProperties createDelegateMorphologicalFilteringStep(QString input, QString output,
                                                                              med::pipeline::MorphologicalFilterType filterType,
                                                                              med::pipeline::RetryMode retryMode = med::pipeline::MULTIPLE_TRIES,
                                                                              med::pipeline::RadiusType radiusType = med::pipeline::RADIUS_IN_MM_OR_PIXELS,
                                                                              int initialRadius = -1);
    void startMorphologicalFilteringStep(PipelineWorkingState* state);
    void endMorphologicalFilteringStep(PipelineWorkingState* state);

private:
    FilteringDelegatePrivate* const d;

    void startFilteringStep(dtkSmartPointer<medAbstractData> input, int filterType,
                            med::pipeline::RetryMode retryMode = med::pipeline::SINGLE_TRY);
    void setupViewParameterGroup(QString name);
    void setupSelectorToolBox(QString toolBoxIdentifier, dtkSmartPointer<medAbstractData> input);

    dtkSmartPointer<medAbstractData> getFilteredImaged();

    QString getFilterToolBoxIdentifier(int filterType);
    QString getFilterComboBoxName(int filterType);
    int getFilterComboBoxIndex(int filterType);

    bool isBasicFilter(int filterType);
    bool isMorphologicalFilter(int filterType);

private slots:
    void displayBasicFilteringResult();
    void displayMorphologicalResult();
};

} // namespace med::pipeline
