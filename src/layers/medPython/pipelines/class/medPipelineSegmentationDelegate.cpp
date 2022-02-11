#include "medPipelineSegmentationDelegate.h"

#include <itkImageDuplicator.h>

#include <dtkCoreSupport/dtkAbstractProcessFactory>

#include <medAbstractProcessLegacy.h>
#include <medAbstractDataFactory.h>
#include <medClutEditorToolBox.h>
#include <medDataManager.h>
#include <medDoubleParameterL.h>
#include <medMetaDataKeys.h>
#include <medTabbedViewContainers.h>
#include <medUtilities.h>
#include <medViewContainer.h>
#include <statsROI.h>

#include "medPipelineControlToolBox.h"
#include "medPipelineFactory.h"
#include "medPipelineTypes.h"
#include "medPipelineWorkspace.h"
#include "medPipelineRoiToolBox.h"

#if defined ( WIN32 )
#define __func__ __FUNCTION__
#endif

namespace med::pipeline
{

class SegmentationDelegatePrivate
{
public:
    dtkSmartPointer<medAbstractData> input;
    dtkSmartPointer<medAbstractData> mask;
    QList<dtkSmartPointer<medAbstractData> > paintMasks;
    RetryMode retryMode;
};

void SegmentationDelegate::registerDelegate()
{
    PipelineFactory::instance()->registerDelegate<SegmentationDelegate>();
}

SegmentationDelegate::SegmentationDelegate(void* argument)
    : d(new SegmentationDelegatePrivate())
{
}

SegmentationDelegate::~SegmentationDelegate()
{
    delete d;
}

PipelineStepProperties SegmentationDelegate::createPaintStep(QString name, QString regionName,
                                                                           RetryMode retryMode)
{
    PipelineStepProperties stepProperties = PipelineStepProperties(name);

    if (regionName == "")
    {
        stepProperties.setTitle("Paint segmentation");
        stepProperties.setDescription("Segment with the brush.");
    }
    else
    {
        stepProperties.setTitle(asTitle(regionName) + " segmentation");
        stepProperties.setDescription(QString("Segment ") + regionName + " with the brush.");
    }

    if (retryMode == MULTIPLE_TRIES)
    {
        stepProperties.setDescription(stepProperties.getDescription() + " After each segmentation, 'Save' the mask.");
        stepProperties.setResultDropMode(MULTIPLE_RESULT_DROPS);
    }
    else
    {
        stepProperties.setResultDropMode(SINGLE_RESULT_DROP);
    }

    return stepProperties;
}

void SegmentationDelegate::startPaintStep(dtkSmartPointer<medAbstractData> input, RetryMode mode)
{
    d->input = input;
    d->retryMode = mode;

    medToolBox* paintToolBox = getWorkspace()->addToolBox(PAINT_TOOLBOX);

    getWorkspace()->addData(input);
    getWorkspace()->setToolBoxVisibility(MOUSE_INTERACTION_TOOLBOX, true);
    getWorkspace()->setToolBoxVisibility(VIEW_SETTINGS_TOOLBOX, true);
    getWorkspace()->setToolBoxVisibility(LAYERS_TOOLBOX, true);

    paintToolBox->getComponent<QAbstractButton*>("paintButton")->click();
    paintToolBox->getComponent<QAbstractButton*>("Magic Wand")->hide();

    QAbstractButton* saveButton = paintToolBox->getComponent<QAbstractButton*>("saveButton");

    if (d->retryMode == MULTIPLE_TRIES)
    {
        disconnect(saveButton, SIGNAL(pressed()), paintToolBox, nullptr);
        connect(saveButton, SIGNAL(clicked()), this, SLOT(retrievePaintedMask()));
    }
    else
    {
        saveButton->hide();
        saveButton->setDisabled(true);
    }
}

void SegmentationDelegate::endPaintStep(QList<dtkSmartPointer<medAbstractData> >* outputs, OutputType outputType)
{
    if (d->retryMode == SINGLE_TRY)
    {
        (*outputs).append(getWorkspace()->getDroppedResultOrToolBoxOutput(PAINT_TOOLBOX));
    }
    else
    {
        *outputs = getWorkspace()->getDroppedResults();
        (*outputs).append(d->paintMasks);
    }

    for (int i = 0; i < outputs->length(); i++)
    {
        (*outputs)[i] = convertToOutputType((*outputs)[i], outputType, d->input);
    }
}

PipelineStepProperties SegmentationDelegate::createPolygonROIStep(QString name, QString regionName)
{
    return PipelineStepProperties(name)
            .setTitle(QString(asTitle(regionName)) + " segmentation")
            .setDescription(QString("Segment ") + regionName + " with Polygon Roi tools.")
            .setResultDropMode(SINGLE_RESULT_DROP);
}

void SegmentationDelegate::startPolygonROIStep(dtkSmartPointer<medAbstractData> input)
{
    d->input = input;
    medToolBox* polygonRoiToolBox = getWorkspace()->addToolBox(POLYGON_ROI_TOOLBOX);
    polygonRoiToolBox->getComponent<QAbstractButton*>("generateBinaryImageButton")->hide();
    getWorkspace()->addData(input);
    getWorkspace()->setToolBoxVisibility(MOUSE_INTERACTION_TOOLBOX, true);
    getWorkspace()->setToolBoxVisibility(LAYERS_TOOLBOX, true);
    polygonRoiToolBox->getComponent<QAbstractButton*>("closedPolygonButton")->click();
}

void SegmentationDelegate::endPolygonROIStep(dtkSmartPointer<medAbstractData>* output, OutputType outputType)
{
    *output = convertToOutputType(getWorkspace()->getDroppedResultOrToolBoxOutput(POLYGON_ROI_TOOLBOX), outputType, d->input);
}

PipelineStepProperties SegmentationDelegate::createPolygonROISamplingStep(QString name, QString regionName)
{
    return PipelineStepProperties(name)
            .setTitle(asTitle(regionName) + " sample")
            .setDescription(QString("Segment a small part of the ") + regionName + " on one slice.");
}

void SegmentationDelegate::startPolygonROISamplingStep(dtkSmartPointer<medAbstractData> input)
{
    startPolygonROIStep(input);
    getWorkspace()->getToolBox(POLYGON_ROI_TOOLBOX)->getComponent<QAbstractButton*>("interpolateButton")->hide();
}

void SegmentationDelegate::endPolygonROISamplingStep(QList<double>* outputValues)
{
    statsROI statsProcess;
    statsProcess.setInput(getWorkspace()->getLayerData(), 0); //data
    statsProcess.setInput(getWorkspace()->getToolBoxOutput(POLYGON_ROI_TOOLBOX), 1); //mask
    statsProcess.setParameter(statsROI::MEAN_STDDEVIATION);
    statsProcess.update();
    *outputValues = QList<double>::fromVector(QVector<double>::fromStdVector(statsProcess.output()));
}

PipelineStepProperties SegmentationDelegate::createDraftMagicWandStep(QString name, QString regionName)
{
    return createMagicWandStep(name, regionName);
}

void SegmentationDelegate::startDraftMagicWandStep(dtkSmartPointer<medAbstractData> input,
                                                              QList<double> innerSampleStats, QList<double> outerSampleStats)
{
    startMagicWandStep(input, innerSampleStats, outerSampleStats);
}

void SegmentationDelegate::endDraftMagicWandStep(QList<double>* outputStats)
{
    outputStats->clear();

    if (getWorkspace()->getToolBoxOutput(PAINT_TOOLBOX))
    {
        statsROI statsProcess;
        statsProcess.setInput(getWorkspace()->getLayerData(), 0); //data
        statsProcess.setInput(getWorkspace()->getToolBoxOutput(PAINT_TOOLBOX), 1); //mask
        statsProcess.update();
        *outputStats = QList<double>::fromVector(QVector<double>::fromStdVector(statsProcess.output()));
    }
}

PipelineStepProperties SegmentationDelegate::createMagicWandStep(QString name, QString regionName)
{
    return PipelineStepProperties(name)
            .setTitle(asTitle(regionName) + " segmentation")
            .setDescription(QString("Use magic wand to segment the ") + regionName + ".")
            .setResultDropMode(SINGLE_RESULT_DROP);
}

void SegmentationDelegate::startMagicWandStep(dtkSmartPointer<medAbstractData> input)
{
    d->input = input;

    medToolBox* paintToolBox = getWorkspace()->addToolBox(PAINT_TOOLBOX);

    paintToolBox->getComponent<QAbstractButton*>("paintButton")->hide();

    getWorkspace()->addData(input);

    paintToolBox->getComponent<QAbstractButton*>("Magic Wand")->click();
    paintToolBox->getComponent<QAbstractButton*>("Activate 3D mode")->setChecked(true);

    getWorkspace()->setToolBoxVisibility(MOUSE_INTERACTION_TOOLBOX, true);
}

void SegmentationDelegate::startMagicWandStep(dtkSmartPointer<medAbstractData> input,
                                                         QList<double> innerSampleStats, QList<double> outerSampleStats)
{
    double magicThreshold = outerSampleStats[0] + outerSampleStats[1] * ((innerSampleStats[0] - outerSampleStats[0]) / (innerSampleStats[1] + outerSampleStats[1]));
    startMagicWandStep(input, magicThreshold, std::numeric_limits<double>::max()); // max of type double is used to be sure to get the maximun of the data (human CT should not have value so high)
}

void SegmentationDelegate::startMagicWandStep(dtkSmartPointer<medAbstractData> input, double minThreshold, double maxThreshold)
{  
    startMagicWandStep(input);
    medToolBox* paintToolBox = getWorkspace()->getToolBox(PAINT_TOOLBOX);

    medDoubleParameterL* lowerThresholdSlider = paintToolBox->getComponent<medDoubleParameterL*>("Lower Threshold");
    medDoubleParameterL* upperThresholdSlider = paintToolBox->getComponent<medDoubleParameterL*>("Upper Threshold");

    if(minThreshold >= lowerThresholdSlider->getSpinBox()->minimum() && minThreshold <= maxThreshold && minThreshold <= lowerThresholdSlider->getSpinBox()->maximum())
    {
        lowerThresholdSlider->setValue(minThreshold);
    }
    else
    {
        lowerThresholdSlider->setValue(lowerThresholdSlider->getSpinBox()->minimum());
    }

    if(maxThreshold <= upperThresholdSlider->getSpinBox()->maximum() && maxThreshold >= minThreshold && maxThreshold >= upperThresholdSlider->getSpinBox()->minimum())
    {
        upperThresholdSlider->setValue(maxThreshold);
    }
    else
    {
        upperThresholdSlider->setValue(upperThresholdSlider->getSpinBox()->maximum());
    }
}

void SegmentationDelegate::endMagicWandStep(dtkSmartPointer<medAbstractData>* output, OutputType outputType)
{
    *output = convertToOutputType(getWorkspace()->getDroppedResultOrToolBoxOutput(PAINT_TOOLBOX), outputType, d->input);
}

PipelineStepProperties SegmentationDelegate::createMaskPaintCorrectionStep(QString name, QString regionName)
{
    return PipelineStepProperties(name)
            .setTitle(asTitle(regionName) + " mask correction")
            .setDescription(QString("Use Paint segmentation to correct ") + regionName + ".")
            .setOption("skip",false)
            .setResultDropMode(SINGLE_RESULT_DROP);
}

void SegmentationDelegate::startMaskPaintCorrectionStep(dtkSmartPointer<medAbstractData> mask, dtkSmartPointer<medAbstractData> background)
{
    d->input = mask;

    medToolBox* paintToolBox = getWorkspace()->addToolBox(PAINT_TOOLBOX);

    paintToolBox->getComponent<QAbstractButton*>("saveButton")->hide();

    getWorkspace()->addData(mask);
    getWorkspace()->addData(background);

    paintToolBox->getComponent<QAbstractButton*>("Magic Wand")->click();
    paintToolBox->getComponent<QAbstractButton*>("Activate 3D mode")->setChecked(true);
    paintToolBox->setValue("Lower Threshold", 1.0);
    paintToolBox->setValue("Upper Threshold", 1.0);

    getWorkspace()->setToolBoxVisibility(MOUSE_INTERACTION_TOOLBOX, true);
}

void SegmentationDelegate::endMaskPaintCorrectionStep(dtkSmartPointer<medAbstractData>* output)
{
    *output = getWorkspace()->getDroppedResultOrToolBoxOutput(PAINT_TOOLBOX);
}

PipelineStepProperties SegmentationDelegate::createVesselStep(QString name, QString vesselName)
{
    return PipelineStepProperties(name)
            .setTitle(asTitle(vesselName) + " segmentation")
            .setDescription(QString("Segment the ") + vesselName + ".")
            .setResultDropMode(SINGLE_RESULT_DROP);
}

void SegmentationDelegate::startVesselStep(dtkSmartPointer<medAbstractData> input,
                                           double minRadius, double maxRadius, double power)
{
    medToolBox* toolBox = getWorkspace()->addToolBox(VESSEL_SEGMENTATION_TOOLBOX);

    getWorkspace()->addData(input);

    toolBox->getComponent<QAbstractButton*>("startSegmentationButton")->click();
    toolBox->getComponent<QAbstractButton*>("stopSegmentationButton")->hide();
    toolBox->getComponent<QAbstractButton*>("saveButton")->hide();
    toolBox->setValue("maxRadiusSpinBox", maxRadius);
    toolBox->setValue("minRadiusSpinBox", minRadius);
    toolBox->setValue("powerSpinBox", power);

    getWorkspace()->setToolBoxVisibility(MOUSE_INTERACTION_TOOLBOX, true);
    getWorkspace()->setToolBoxVisibility(VIEW_SETTINGS_TOOLBOX, true);
    getWorkspace()->setToolBoxVisibility(LAYERS_TOOLBOX, false);
}

void SegmentationDelegate::endVesselStep(dtkSmartPointer<medAbstractData>* output)
{
    *output = getWorkspace()->getDroppedResultOrToolBoxOutput(VESSEL_SEGMENTATION_TOOLBOX);
}

PipelineStepProperties SegmentationDelegate::createHistogramAnalysisStep(QString name)
{
    return PipelineStepProperties(name)
            .setTitle("Histogram Analysis");
}

void SegmentationDelegate::startHistogramAnalysisStep(dtkSmartPointer<medAbstractData> image,
                                                      dtkSmartPointer<medAbstractData> mask,
                                                      ThresholdType defaultThresholdType,
                                                      QList<double> vertexPos)
{
    medToolBox* histogramToolBox = getWorkspace()->addToolBox(HISTOGRAM_ANALYSIS_TOOLBOX);
    histogramToolBox->getComponent<QAbstractButton*>("createContainersButton")->hide();

    getWorkspace()->addData(image);
    getWorkspace()->addData(mask);

    histogramToolBox->getComponent<QAbstractButton*>("createContainersButton")->click();

    if (defaultThresholdType == UPPER_THRESHOLD)
    {
        histogramToolBox->getComponent<QAbstractButton*>("Upper Threshold")->click();
    }

    histogramToolBox->getComponent<QAbstractButton*>("Save")->hide();

    if ( !vertexPos.empty() )
    {
        medClutEditorToolBox* editorToolBox = histogramToolBox->getComponent<medClutEditorToolBox*>("clut editor");
        medClutEditorScene* scene = editorToolBox->getScene();
        medClutEditorTable* table = scene->table();
        QPointF value = QPointF(vertexPos[0], vertexPos[1]);
        qDebug()<<"value : "<<value;
        QPointF coord = scene->valueToCoordinate(value);
        table->addVertex(new medClutEditorVertex(value, coord, QColor(Qt::yellow), table));
    }

}

void SegmentationDelegate::endHistogramAnalysisStep(QList<double>* thresholds,
                                                    ThresholdType* chosenThresholdType)
{
    medToolBox* toolBox = getWorkspace()->getToolBox(HISTOGRAM_ANALYSIS_TOOLBOX);

    QList<QVariant> genericThresholds = toolBox->getValue<QList<QVariant> >("tresholds");
    thresholds->reserve(genericThresholds.size());
    std::transform(genericThresholds.begin(), genericThresholds.end(), thresholds->begin(),
                   [] (QVariant element) -> double { return element.toDouble(); });

    QAbstractButton *lowerThresh = toolBox->getComponent<QAbstractButton*>("Lower Threshold");
    *chosenThresholdType = lowerThresh->isChecked() ? LOWER_THRESHOLD : UPPER_THRESHOLD;
}

PipelineStepProperties SegmentationDelegate::createVOICutterStep(QString name)
{
    return PipelineStepProperties(name)
            .setTitle("VOI cutter")
            .setDescription("Cut unwanted elements.")
            .setResultDropMode(SINGLE_RESULT_DROP);
}

void SegmentationDelegate::startVOICutterStep(dtkSmartPointer<medAbstractData> input)
{
    medToolBox* voiCutterToolBox = getWorkspace()->addToolBox(VOI_CUTTER_TOOLBOX);
    voiCutterToolBox->getComponent<QAbstractButton*>("Save Image")->hide();

    getWorkspace()->addData(input);
}

void SegmentationDelegate::endVOICutterStep(dtkSmartPointer<medAbstractData>* output)
{
    *output = getWorkspace()->getDroppedResultOrToolBoxOutput(VOI_CUTTER_TOOLBOX);
}

PipelineStepProperties SegmentationDelegate::createGeodesicActiveContourLevelSetStep(QString name)
{
    return PipelineStepProperties(name)
            .setTitle("Level Set")
            .setDescription("Level Set-Based Segmentation.");
}

void SegmentationDelegate::startGeodesicActiveContourLevelSetStep(dtkSmartPointer<medAbstractData> image, dtkSmartPointer<medAbstractData> mask,
                                                                  double propagation, double curvature, double advection)
{
    medToolBox* toolBox = getWorkspace()->addToolBox(LEVEL_SET_SEGMENTATION_TOOLBOX);

    getWorkspace()->addData(image);
    getWorkspace()->addData(mask);
    toolBox->findChild<QComboBox*>("levelset")->setCurrentIndex(1);
    toolBox->findChild<QComboBox*>("feature")->setCurrentIndex(2);
    toolBox->setValue("propagation", propagation);
    toolBox->setValue("curvature", curvature);
    toolBox->setValue("advection", advection);

    toolBox->getComponent<QAbstractButton*>("Run")->hide();
    getWorkspace()->setToolBoxVisibility(MOUSE_INTERACTION_TOOLBOX, true);
    getWorkspace()->setToolBoxVisibility(VIEW_SETTINGS_TOOLBOX, true);
    getWorkspace()->setToolBoxVisibility(LAYERS_TOOLBOX, false);
}

void SegmentationDelegate::endGeodesicActiveContourLevelSetStep(dtkSmartPointer<medAbstractData> *output)
{
    *output = getWorkspace()->getDroppedResultOrToolBoxOutput(LEVEL_SET_SEGMENTATION_TOOLBOX);
}

PipelineStepProperties SegmentationDelegate::createMaskPaintOperationsStep(QString name, QString regionName)
{
    return PipelineStepProperties(name)
            .setTitle(asTitle(regionName) + " mask operations")
            .setDescription(QString("Use Paint segmentation to correct ") + regionName + ".")
            .setOption("skip",false)
            .setResultDropMode(SINGLE_RESULT_DROP);
}

void SegmentationDelegate::startMaskPaintOperationsStep(dtkSmartPointer<medAbstractData> input, dtkSmartPointer<medAbstractData> mask)
{
    d->input = input;
    d->mask = mask;

    medToolBox* paintToolBox = getWorkspace()->addToolBox(PAINT_TOOLBOX);

    getWorkspace()->addData(input);
    getWorkspace()->addData(mask);

    paintToolBox->getComponent<QAbstractButton*>("paintButton")->click();
    paintToolBox->getComponent<QAbstractButton*>("Magic Wand")->hide();

    QAbstractButton* saveButton = paintToolBox->getComponent<QAbstractButton*>("saveButton");
    saveButton->hide();
    saveButton->setDisabled(true);
    QAbstractButton* addButton = paintToolBox->getComponent<QAbstractButton*>("addButton");
    QAbstractButton* eraseButton = paintToolBox->getComponent<QAbstractButton*>("eraseButton");
    addButton->show();
    eraseButton->show();

    disconnect(addButton, SIGNAL(clicked()), paintToolBox, nullptr);
    disconnect(eraseButton, SIGNAL(clicked()), paintToolBox, nullptr);
    connect(addButton, &QPushButton::clicked, [=] { operationsOnPaintedMask(ADD_MASK); });
    connect(addButton, &QPushButton::clicked, [=] { operationsOnPaintedMask(ERASE_MASK); });
}

void SegmentationDelegate::endMaskPaintOperationsStep(dtkSmartPointer<medAbstractData>* output)
{
    if (getWorkspace()->getDroppedResult() != nullptr )
    {
        *output = getWorkspace()->getDroppedResult();
    }
    else
    {
        *output = d->mask;
    }
    return;
}

PipelineStepProperties SegmentationDelegate::createDelegatePaintStep(QString regionName, QString input,
                                                                     QString output, OutputType outputType,
                                                                     RetryMode retryMode)
{
    return createPaintStep("Paint", regionName, retryMode)
            .setDelegateClass(staticMetaObject.className())
            .requires(input)
            .setOption("input", input)
            .setOption("output", output)
            .setOption<int>("outputType", outputType)
            .setOption<int>("retryMode", retryMode);
}

void SegmentationDelegate::startPaintStep(PipelineWorkingState* state)
{
    dtkSmartPointer<medAbstractData> input = state->getValue(state->getStepOption<QString>("input"));
    RetryMode retryMode = static_cast<RetryMode>(state->getStepOption<int>("retryMode"));
    startPaintStep(input, retryMode);
}

void SegmentationDelegate::endPaintStep(PipelineWorkingState* state)
{
    QList<dtkSmartPointer<medAbstractData> > masks;
    OutputType outputType = static_cast<OutputType>(state->getStepOption<int>("outputType"));
    endPaintStep(&masks, outputType);

    if (!masks.empty())
    {
        for (int i = 0; i < masks.length(); i++)
        {
            QString maskId = state->getStepOption<QString>("output");
            if (masks.length() > 1)
            {
                maskId += " " + QString::number(i);
            }
            state->setValue(maskId, masks[i]);
            if( outputType == IMAGE || outputType == COMPLEMENT_IMAGE)
            {
                state->copyWindowLevel(maskId, state->getStepOption<QString>("input"));
            }
        }
    }
    else
    {
        state->reject("Error during paint.");
    }
}

PipelineStepProperties SegmentationDelegate::createDelegatePolygonROIStep(QString regionName, QString input,
                                                                          QString output, OutputType outputType)
{
    return createPolygonROIStep("PolygonROI", regionName)
            .setDelegateClass(staticMetaObject.className())
            .requires(input)
            .setOption("input", input)
            .setOption("output", output)
            .setOption<int>("outputType", outputType);
}

void SegmentationDelegate::startPolygonROIStep(PipelineWorkingState* state)
{
    startPolygonROIStep(state->getValue(state->getStepOption<QString>("input")));
}

void SegmentationDelegate::endPolygonROIStep(PipelineWorkingState* state)
{
    dtkSmartPointer<medAbstractData> output;
    OutputType outputType = static_cast<OutputType>(state->getStepOption<int>("outputType"));
    endPolygonROIStep(&output, outputType);
    if (output)
    {
        QString outputId = state->getStepOption<QString>("output");
        state->setValue(outputId, output);
        if( outputType == IMAGE || outputType == COMPLEMENT_IMAGE)
        {
            state->copyWindowLevel(outputId, state->getStepOption<QString>("input"));
        }
    }
    else
    {
        state->reject("Error during polygon roi.");
    }
}

PipelineStepProperties SegmentationDelegate::createDelegatePolygonROISamplingStep(QString regionName, QString input, QString output)
{
    return createPolygonROISamplingStep("PolygonROISampling", regionName)
            .setDelegateClass(staticMetaObject.className())
            .requires(input)
            .setOption("input", input)
            .setOption("output", output);
}

void SegmentationDelegate::startPolygonROISamplingStep(PipelineWorkingState* state)
{
    startPolygonROISamplingStep(state->getValue(state->getStepOption<QString>("input")));
}

void SegmentationDelegate::endPolygonROISamplingStep(PipelineWorkingState* state)
{
    QList<double> outputValues;
    endPolygonROISamplingStep(&outputValues);
    if (!outputValues.empty())
    {
        state->setValue(state->getStepOption<QString>("output"), outputValues);
    }
    else
    {
        state->reject("Error during polygon sampling.");
    }
}

PipelineStepProperties SegmentationDelegate::createDelegateDraftMagicWandStep(QString regionName, QString input,
                                                                              QString innerSampleStats,
                                                                              QString outerSampleStats)
{
    return createDraftMagicWandStep("DraftMagicWand", regionName)
            .setDelegateClass(staticMetaObject.className())
            .requires(input)
            .requires(innerSampleStats)
            .requires(outerSampleStats)
            .setOption("input", input)
            .setOption("innerSampleStats", innerSampleStats)
            .setOption("outerSampleStats", outerSampleStats);
}

void SegmentationDelegate::startDraftMagicWandStep(PipelineWorkingState* state)
{
    dtkSmartPointer<medAbstractData> input = state->getValue(state->getStepOption<QString>("input"));
    QList<double> innerSampleStats = state->getValue<QList<double> >(state->getStepOption<QString>("innerSampleStats"));
    QList<double> outerSampleStats = state->getValue<QList<double> >(state->getStepOption<QString>("outerSampleStats"));
    startDraftMagicWandStep(input, innerSampleStats, outerSampleStats);
}

void SegmentationDelegate::endDraftMagicWandStep(PipelineWorkingState* state)
{
    QList<double> improvedInnerSampleStats;
    endDraftMagicWandStep(&improvedInnerSampleStats);

    if (!improvedInnerSampleStats.empty())
    {
        state->setValue(state->getStepOption<QString>("innerSampleStats"), improvedInnerSampleStats);
    }
    else
    {
        state->reject("Error during draft magic wand.");
    }
}

PipelineStepProperties SegmentationDelegate::createDelegateMagicWandStep(QString regionName, QString input,
                                                                                       QString output, OutputType outputType)
{
    return createMagicWandStep("MagicWand", regionName)
            .setDelegateClass(staticMetaObject.className())
            .requires(input)
            .setOption("input", input)
            .setOption("output", output)
            .setOption<int>("outputType", outputType);
}

PipelineStepProperties SegmentationDelegate::createDelegateMagicWandStep(QString regionName, QString input,
                                                                                       QString innerSampleStats,
                                                                                       QString outerSampleStats,
                                                                                       QString output, OutputType outputType)
{
    return createDelegateMagicWandStep(regionName, input, output, outputType)
            .requires(innerSampleStats)
            .requires(outerSampleStats)
            .setOption("innerSampleStats", innerSampleStats)
            .setOption("outerSampleStats", outerSampleStats);
}

PipelineStepProperties SegmentationDelegate::createDelegateMagicWandStep(QString regionName, QString input,
                                                                                       double minThreshold, double maxThreshold,
                                                                                       QString output, OutputType outputType)
{
    return createDelegateMagicWandStep(regionName, input, output, outputType)
            .setOption<double>("minThreshold", minThreshold)
            .setOption<double>("maxThreshold", maxThreshold);
}

void SegmentationDelegate::startMagicWandStep(PipelineWorkingState* state)
{
    dtkSmartPointer<medAbstractData> input = state->getValue(state->getStepOption<QString>("input"));

    if (state->hasStepOption("innerSampleStats"))
    {
        QList<double> innerSampleStats = state->getValue<QList<double> >(state->getStepOption<QString>("innerSampleStats"));
        QList<double> outerSampleStats = state->getValue<QList<double> >(state->getStepOption<QString>("outerSampleStats"));
        startMagicWandStep(input, innerSampleStats, outerSampleStats);
    }
    else if (state->hasStepOption("minThreshold"))
    {
        double minThreshold = state->getStepOption<double>("minThreshold");
        double maxThreshold = state->getStepOption<double>("maxThreshold");
        startMagicWandStep(input, minThreshold, maxThreshold);
    }
    else
    {
        startMagicWandStep(input);
    }
}

void SegmentationDelegate::endMagicWandStep(PipelineWorkingState* state)
{
    dtkSmartPointer<medAbstractData> output;
    OutputType outputType = static_cast<OutputType>(state->getStepOption<int>("outputType"));
    endMagicWandStep(&output, outputType);
    if (output)
    {
        QString outputId = state->getStepOption<QString>("output");
        state->setValue(outputId, output);
        if( outputType == IMAGE || outputType == COMPLEMENT_IMAGE)
        {
            state->copyWindowLevel(outputId, state->getStepOption<QString>("input"));
        }
    }
    else
    {
        state->reject("Error during magic wand.");
    }
}

PipelineStepProperties
SegmentationDelegate::createDelegateMaskPaintCorrectionStep(QString regionName, QString mask,
                                                                       QString background, QString output)
{
    return createMaskPaintCorrectionStep("MaskPaintCorrection", regionName)
            .setDelegateClass(staticMetaObject.className())
            .requires(mask)
            .requires(background)
            .setOption("mask", mask)
            .setOption("background", background)
            .setOption("output", output);
}

void SegmentationDelegate::startMaskPaintCorrectionStep(PipelineWorkingState *state)
{
    dtkSmartPointer<medAbstractData> mask = state->getValue(state->getStepOption<QString>("mask"));
    dtkSmartPointer<medAbstractData> background = state->getValue(state->getStepOption<QString>("background"));
    startMaskPaintCorrectionStep(mask, background);
}

void SegmentationDelegate::endMaskPaintCorrectionStep(PipelineWorkingState *state)
{
    dtkSmartPointer<medAbstractData> output;
    endMaskPaintCorrectionStep(&output);
    if (output)
    {
        QString outputId = state->getStepOption<QString>("output");
        state->setValue(outputId, output);
    }
    else
    {
        state->reject("Error during segmentation.");
    }
}

PipelineStepProperties SegmentationDelegate::createDelegateVesselStep(QString vesselName, QString input,
                                                                                    QString output, QString outputColor,
                                                                                    double minRadius, double maxRadius,
                                                                                    double power)
{
    return createVesselStep("Vessel", vesselName)
            .setDelegateClass(staticMetaObject.className())
            .requires(input)
            .setOption("input", input)
            .setOption("output", output)
            .setOption("outputColor", outputColor)
            .setOption("minRadius", minRadius)
            .setOption("maxRadius", maxRadius)
            .setOption("power", power);
}

void SegmentationDelegate::startVesselStep(PipelineWorkingState* state)
{
    dtkSmartPointer<medAbstractData> input = state->getValue(state->getStepOption<QString>("input"));
    double minRadius = state->getStepOption<double>("minRadius");
    double maxRadius = state->getStepOption<double>("maxRadius");
    double power = state->getStepOption<double>("power");
    startVesselStep(input, minRadius, maxRadius, power);
}

void SegmentationDelegate::endVesselStep(PipelineWorkingState* state)
{
    dtkSmartPointer<medAbstractData> output;
    endVesselStep(&output);
    QString outputId = state->getStepOption<QString>("output");
    state->setValue(outputId, output);
    QString outputColor = state->getStepOption<QString>("outputColor");
    state->setRenderingParameters(outputId, RenderingParameters(RenderingParameters::SURFACE, outputColor, 1.0));
}

PipelineStepProperties SegmentationDelegate::createDelegateHistogramAnalysisStep(QString image, QString mask,
                                                                                               QString output,
                                                                                               ThresholdType defaultThresholdType,
                                                                                               QList<double> vertexPos,
                                                                                               int numThresholds)
{
    return createHistogramAnalysisStep("HistogramAnalysis")
            .setDelegateClass(staticMetaObject.className())
            .requires(image)
            .requires(mask)
            .setOption("image", image)
            .setOption("mask", mask)
            .setOption("output", output)
            .setOption<int>("defaultThresholdType", defaultThresholdType)
            .setOption<int>("numThresholds", numThresholds)
            .setOption<QList<double>>("vertexPos", vertexPos);
}

void SegmentationDelegate::startHistogramAnalysisStep(PipelineWorkingState* state)
{
    dtkSmartPointer<medAbstractData> image= state->getValue(state->getStepOption<QString>("image"));
    dtkSmartPointer<medAbstractData> mask = state->getValue(state->getStepOption<QString>("mask"));
    ThresholdType defaultThresholdType = static_cast<ThresholdType>(state->getStepOption<int>("defaultThresholdType"));
    QList<double> vertexPos = static_cast<QList<double>>(state->getStepOption<QList<double>>("vertexPos"));
    startHistogramAnalysisStep(image, mask, defaultThresholdType, vertexPos);
}

void SegmentationDelegate::endHistogramAnalysisStep(PipelineWorkingState* state)
{
    QList<double> thresholds;
    ThresholdType chosenThresholdType;
    endHistogramAnalysisStep(&thresholds, &chosenThresholdType);

    int numThresholds = state->getStepOption<int>("numThresholds");
    if (thresholds.length() != numThresholds)
    {
        state->reject(QString("There should be ") + QString::number(numThresholds) + " thresholds.");
    }
    else
    {
        QString output = state->getStepOption<QString>("output");
        state->setValue<QList<double> >(output, thresholds);
        state->setValue<int>(output + "Type", chosenThresholdType);
    }
}

PipelineStepProperties SegmentationDelegate::createDelegateVOICutterStep(QString input, QString output)
{
    return createVOICutterStep("VOICutter")
            .setDelegateClass(staticMetaObject.className())
            .requires(input)
            .setOption("input", input)
            .setOption("output", output);
}

void SegmentationDelegate::startVOICutterStep(PipelineWorkingState* state)
{
    dtkSmartPointer<medAbstractData> input = state->getValue(state->getStepOption<QString>("input"));
    startVOICutterStep(input);
}

void SegmentationDelegate::endVOICutterStep(PipelineWorkingState* state)
{
    dtkSmartPointer<medAbstractData> output;
    endVOICutterStep(&output);
    if (output)
    {
        state->setValue(state->getStepOption<QString>("output"), output);
    }
    else
    {
        state->reject("Error during VOI cutter.");
    }
}

PipelineStepProperties SegmentationDelegate::createDelegateGeodesicActiveContourLevelSetStep(QString image, QString mask, QString output,
                                                                                                           double propagation, double curvature, double advection)
{
    return createGeodesicActiveContourLevelSetStep("GeodesicActiveContourLevelSet")
            .setDelegateClass(staticMetaObject.className())
            .requires(image)
            .requires(mask)
            .setOption("image", image)
            .setOption("mask", mask)
            .setOption("output", output)
            .setOption<double>("propagation", propagation)
            .setOption<double>("curvature", curvature)
            .setOption<double>("advection", advection);
}

void SegmentationDelegate::startGeodesicActiveContourLevelSetStep(PipelineWorkingState *state)
{
    dtkSmartPointer<medAbstractData> image= state->getValue(state->getStepOption<QString>("image"));
    dtkSmartPointer<medAbstractData> mask = state->getValue(state->getStepOption<QString>("mask"));
    double propagation = state->getStepOption<double>("propagation");
    double curvature = state->getStepOption<double>("curvature");
    double advection = state->getStepOption<double>("advection");
    startGeodesicActiveContourLevelSetStep(image, mask, propagation, curvature, advection);
}

void SegmentationDelegate::endGeodesicActiveContourLevelSetStep(PipelineWorkingState *state)
{
    dtkSmartPointer<medAbstractData> output;
    endGeodesicActiveContourLevelSetStep(&output);
    if (output)
    {
        state->setValue(state->getStepOption<QString>("output"), output);
    }
    else
    {
        state->reject("Error during Level Set-Based Segmentation.");
    }
}

PipelineStepProperties SegmentationDelegate::createDelegateMaskPaintOperationsStep(QString regionName, QString input, QString mask, QString output)
{
    return createMaskPaintOperationsStep("MaskPaintOperations", regionName)
            .setDelegateClass(staticMetaObject.className())
            .requires(input)
            .requires(mask)
            .setOption("input", input)
            .setOption("mask", mask)
            .setOption("output", output);

}

void SegmentationDelegate::startMaskPaintOperationsStep(PipelineWorkingState *state)
{
    dtkSmartPointer<medAbstractData> input = state->getValue(state->getStepOption<QString>("input"));
    dtkSmartPointer<medAbstractData> mask = state->getValue(state->getStepOption<QString>("mask"));
    startMaskPaintOperationsStep(input, mask);
}

void SegmentationDelegate::endMaskPaintOperationsStep(PipelineWorkingState *state)
{
    dtkSmartPointer<medAbstractData> output;
    endMaskPaintOperationsStep(&output);
    if (output)
    {
        QString outputId = state->getStepOption<QString>("output");
        state->setValue(outputId, output);
    }
    else
    {
        state->reject("Error during paint segmentation.");
    }

}

QString SegmentationDelegate::asTitle(QString name)
{
    QString result = name.simplified().toLower();
    result[0] = result[0].toUpper();
    return result;
}

dtkSmartPointer<medAbstractData> SegmentationDelegate::convertToOutputType(dtkSmartPointer<medAbstractData> mask,
                                                                                      OutputType outputType,
                                                                                      dtkSmartPointer<medAbstractData> sourceImage)
{
    dtkSmartPointer<medAbstractData> res = nullptr;
    if(mask)
    {
        switch (outputType)
        {
        case MASK:
        {
            res = mask;
            break;
        }
        case IMAGE:
        {
            res = PipelineUtils::mask(sourceImage, mask);
            break;
        }
        case COMPLEMENT_MASK:
        {
            res = PipelineUtils::notImage(mask);
            break;
        }
        case COMPLEMENT_IMAGE:
        {
            res = PipelineUtils::mask(sourceImage, PipelineUtils::notImage(mask));
            break;
        }
        default:
        {
            throw std::invalid_argument("SegmentationDelegate::convertToOutputType wrong type given.");
        }
        }
    }

    return res;
}

void SegmentationDelegate::retrievePaintedMask()
{
    dtkSmartPointer<medAbstractData> data = getWorkspace()->getToolBoxOutput("med::AlgorithmPaintToolBox");
    dtkSmartPointer<medAbstractData> dataCopy = duplicateMask(data);
    medUtilities::setDerivedMetaData(dataCopy, d->input, "painted");

    d->paintMasks.append(dataCopy);

    medToolBox* toolBox = getWorkspace()->getToolBox(PAINT_TOOLBOX);
    toolBox->getComponent<QAbstractButton*>("clearMaskButton")->click();
    toolBox->getComponent<QAbstractButton*>("paintButton")->click();
    toolBox->getComponent<QAbstractButton*>("saveButton")->show();
    toolBox->getComponent<QAbstractButton*>("saveButton")->setDisabled(true);
    toolBox->getComponent<QAbstractButton*>("clearMaskButton")->show();
}


void SegmentationDelegate::operationsOnPaintedMask(int operationType)
{
    dtkSmartPointer<medAbstractData> data = getWorkspace()->getDroppedResultOrToolBoxOutput(PAINT_TOOLBOX);
    dtkSmartPointer<medAbstractData> dataCopy = duplicateMask(d->mask);
    int layer = getWorkspace()->getView()->layer(d->mask);

    switch (operationType)
    {
    case ADD_MASK:
    {
        d->mask = PipelineUtils::orImage(dataCopy, data);
        break;
    }
    case ERASE_MASK:
    {
        d->mask = PipelineUtils::andImage(dataCopy, PipelineUtils::notImage(data));
        break;
    }
    default:
    {
        throw std::invalid_argument("SegmentationDelegate::operationsOnPaintedMask wrong operation type given.");
    }
    }

    getWorkspace()->getView()->removeLayer(layer);

    d->mask->setMetaData( medMetaDataKeys::SeriesDescription.key(),  "updatedMask");
    getWorkspace()->getView()->insertLayer(layer, d->mask);

    medToolBox* toolBox = getWorkspace()->getToolBox(PAINT_TOOLBOX);
    toolBox->getComponent<QAbstractButton*>("clearMaskButton")->click();
    toolBox->getComponent<QAbstractButton*>("paintButton")->click();
    toolBox->getComponent<QAbstractButton*>("addButton")->show();
    toolBox->getComponent<QAbstractButton*>("addButton")->setDisabled(true);
    toolBox->getComponent<QAbstractButton*>("eraseButton")->show();
    toolBox->getComponent<QAbstractButton*>("eraseButton")->setDisabled(true);
    toolBox->getComponent<QAbstractButton*>("clearMaskButton")->show();
}

dtkSmartPointer<medAbstractData> SegmentationDelegate::duplicateMask(dtkSmartPointer<medAbstractData> data )
{
    typedef itk::Image<unsigned char, 3> MaskType;
    typedef itk::ImageDuplicator<MaskType> DuplicatorType;
    DuplicatorType::Pointer duplicator = DuplicatorType::New();
    duplicator->SetInputImage(static_cast<MaskType*>(data->data()));
    duplicator->Update();
    MaskType::Pointer imageCopy = duplicator->GetOutput();
    dtkSmartPointer<medAbstractData> dataCopy = medAbstractDataFactory::instance()->createSmartPointer("itkDataImageUChar3");
    dataCopy->setData(imageCopy);

    return dataCopy;
}

} // namespace med::pipeline
