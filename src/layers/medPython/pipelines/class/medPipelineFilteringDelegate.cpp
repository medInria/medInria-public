#include "medPipelineFilteringDelegate.h"

#include <medAbstractSelectableToolBox.h>
#include <medDoubleParameterL.h>
#include <medIntParameterL.h>
#include <medSelectorToolBox.h>
#include <medTabbedViewContainers.h>
#include <medToolBox.h>
#include <medUtilities.h>
#include <medUtilitiesITK.h>
#include <medViewContainer.h>
#include <medViewParameterGroupL.h>

#include "medPipelineFactory.h"
#include "medPipelineControlToolBox.h"
#include "medPipelineWorkspace.h"

namespace
{

const char* filterNames[] =
{
    "Addition",
    "Subtraction",
    "Multiplication",
    "Division",
    "Gaussian blur",
    "Normalization",
    "Median",
    "Inversion",
    "Shrinking",
    "Intensity windowing",
    "Thresholding",
    "Connectivity thresholding",
    "Dilation",
    "Erosion",
    "Binary Closing",
    "Binary Opening",
    "Grayscale Closing",
    "Grayscale Opening",
    "Filter"
};

}

namespace med::pipeline
{

class FilteringDelegatePrivate
{
public:
    QSharedPointer<medViewParameterGroupL> viewGroup;
    QSharedPointer<medSelectorToolBox> selectorToolBox;
    dtkSmartPointer<medAbstractData> input;
    int filterType;
    RetryMode retryMode;
};

void FilteringDelegate::registerDelegate()
{
    PipelineFactory::instance()->registerDelegate<FilteringDelegate>();
}

FilteringDelegate::FilteringDelegate(void* argument)
    : d(new FilteringDelegatePrivate())
{
}

FilteringDelegate::~FilteringDelegate()
{
    delete d;
}

PipelineStepProperties FilteringDelegate::createSizeThresholdingStep(QString name)
{
    return PipelineStepProperties(name)
            .setTitle(filterNames[SIZE_THRESHOLDING])
            .setDescription("Remove small elements.");
}

void FilteringDelegate::startSizeThresholdingStep(dtkSmartPointer<medAbstractData> input, RetryMode retryMode)
{
    startFilteringStep(input, SIZE_THRESHOLDING, retryMode);
    getWorkspace()->setVolumeRendering();
}

void FilteringDelegate::endSizeThresholdingStep(dtkSmartPointer<medAbstractData>* output, double* chosenThreshold)
{
    if (chosenThreshold)
    {
        *chosenThreshold = getWorkspace()->getToolBox(ITK_FILTERS_TOOLBOX)->getValue<double>("componentSizeThresholdFilterValue");
    }

    *output = getFilteredImaged();
}

PipelineStepProperties FilteringDelegate::createThresholdingStep(QString name)
{
    return PipelineStepProperties(name)
            .setTitle(filterNames[THRESHOLDING])
            .setDescription("Threshold an Image.");
}

void FilteringDelegate::startThresholdingStep(dtkSmartPointer<medAbstractData> input,
                                              double thresholdValue, int outsideValue,
                                              ThresholdType thresholdType, bool showHistogram,
                                              double lowerValue, double upperValue)
{
    startFilteringStep(input, THRESHOLDING, SINGLE_TRY);
    medToolBox* toolBox = getWorkspace()->getToolBox(getFilterToolBoxIdentifier(THRESHOLDING));

    toolBox->getComponent<QAbstractButton*>("Run")->hide();

    medDoubleParameterL* lower = toolBox->getComponent<medDoubleParameterL*>("lowerValue");
    medDoubleParameterL* upper = toolBox->getComponent<medDoubleParameterL*>("upperValue");
    medDoubleParameterL* threshold = toolBox->getComponent<medDoubleParameterL*>("thresholdValue");

    if (lowerValue == 0 && upperValue == 0 )
    {
        lowerValue = medUtilitiesITK::minimumValue(input);
        upperValue = medUtilitiesITK::maximumValue(input);
    }

    lower->blockSignals(true);
    upper->blockSignals(true);
    lower->setValue(lowerValue);
    upper->setValue(upperValue);
    lower->blockSignals(false);
    upper->blockSignals(false);
    threshold->blockSignals(true);
    threshold->setValue(thresholdValue);
    threshold->blockSignals(false);

    toolBox->setValue("outsideThresholdValue", outsideValue);

    QString thresholdButtonName;

    switch (thresholdType)
    {
    case OUTSIDE_THRESHOLD:
    {
        thresholdButtonName = "outsideButton";
        break;
    }
    case LOWER_THRESHOLD:
    {
        thresholdButtonName = "lowerButton";
        break;
    }
    case UPPER_THRESHOLD:
    {
        thresholdButtonName = "greaterButton";
        break;
    }
    default:
    {
        THROW_METHOD_EXCEPTION("Invalid threshold type: " + QString::number(thresholdType));
    }
    }

    toolBox->getComponent<QAbstractButton*>(thresholdButtonName)->click();
    toolBox->getComponent<QAbstractButton*>("histogram")->setChecked(showHistogram);

    getWorkspace()->setToolBoxVisibility(MOUSE_INTERACTION_TOOLBOX, true);
    getWorkspace()->setToolBoxVisibility(VIEW_SETTINGS_TOOLBOX, false);
    getWorkspace()->setToolBoxVisibility(LAYERS_TOOLBOX, false);
}

void FilteringDelegate::endThresholdingStep(dtkSmartPointer<medAbstractData> *output)
{
    *output = getFilteredImaged();
}

PipelineStepProperties FilteringDelegate::createBinaryThresholdingStep(QString name)
{
    return PipelineStepProperties(name)
            .setTitle(name)
            .setDescription("Binarize an Image by thresholding.");
}

void FilteringDelegate::startBinaryThresholdingStep(dtkSmartPointer<medAbstractData> input,
                                                    int insideValue, int outsideValue,
                                                    bool showHistogram,
                                                    double lowerValue, double upperValue)
{
    startFilteringStep(input, THRESHOLDING, SINGLE_TRY);
    medToolBox* toolBox = getWorkspace()->getToolBox(getFilterToolBoxIdentifier(THRESHOLDING));
    toolBox->getComponent<QAbstractButton*>("binaryThresholdButton")->click();

    if (lowerValue == 0 && upperValue == 0 )
    {
        lowerValue = medUtilitiesITK::minimumValue(input);
        upperValue = medUtilitiesITK::maximumValue(input);
    }

    toolBox->getComponent<QAbstractButton*>("Run")->hide();

    toolBox->setValue("lowerValue", lowerValue);
    toolBox->setValue("upperValue", upperValue);
    toolBox->setValue("thresholdValue", (double)insideValue);
    toolBox->setValue("outsideThresholdValue", outsideValue);
    toolBox->getComponent<QAbstractButton*>("histogram")->setChecked(showHistogram);

    getWorkspace()->setToolBoxVisibility(MOUSE_INTERACTION_TOOLBOX, true);
    getWorkspace()->setToolBoxVisibility(VIEW_SETTINGS_TOOLBOX, false);
    getWorkspace()->setToolBoxVisibility(LAYERS_TOOLBOX, false);
}

void FilteringDelegate::endBinaryThresholdingStep(dtkSmartPointer<medAbstractData> *output)
{
    *output = getFilteredImaged();
}

PipelineStepProperties FilteringDelegate::createMorphologicalFilteringStep(QString name,
                                                                           MorphologicalFilterType filterType)
{
    return PipelineStepProperties(name)
            .setTitle(filterNames[filterType])
            .setDescription(QString("Select the ") + QString(filterNames[filterType]).toLower() + " radius.");
}

void FilteringDelegate::startMorphologicalFilteringStep(dtkSmartPointer<medAbstractData> input,
                                                        MorphologicalFilterType filterType, RetryMode retryMode,
                                                        RadiusType radiusType, int initialRadius)
{
    startFilteringStep(input, filterType, retryMode);

    QString toolBoxIdentifier = getFilterToolBoxIdentifier(filterType);
    medToolBox* toolBox = getWorkspace()->getToolBox(toolBoxIdentifier);

    if (retryMode == SINGLE_TRY)
    {
        toolBox->getComponent<QAbstractButton*>("Run")->hide();
    }

    QAbstractButton* mmButton = toolBox->getComponent<QAbstractButton*>("mm");
    QAbstractButton* pixelsButton = toolBox->getComponent<QAbstractButton*>("pixels");

    switch (radiusType)
    {
    case RADIUS_IN_MM:
        mmButton->setChecked(true);
        mmButton->setDisabled(true);
        pixelsButton->hide();
        break;
    case RADIUS_IN_PIXELS:
        pixelsButton->setChecked(true);
        pixelsButton->setDisabled(true);
        mmButton->hide();
        break;
    case RADIUS_IN_MM_OR_PIXELS:
        break;
    }

    if (initialRadius >= 0)
    {
        toolBox->setValue("kernelSize", initialRadius);
    }

    getWorkspace()->setToolBoxVisibility(MOUSE_INTERACTION_TOOLBOX, true);
    getWorkspace()->setToolBoxVisibility(VIEW_SETTINGS_TOOLBOX, false);
    getWorkspace()->setToolBoxVisibility(LAYERS_TOOLBOX, false);
}

void FilteringDelegate::endMorphologicalFilteringStep(dtkSmartPointer<medAbstractData>* output, int* chosenRadius,
                                                      RadiusType* chosenRadiusType)
{
    medToolBox* toolBox = getWorkspace()->getToolBox(ITK_MORPHOLOGICAL_FILTERS_TOOLBOX);

    if (chosenRadius)
    {
        *chosenRadius = toolBox->getValue<int>("kernelSize");
    }

    if (chosenRadiusType)
    {
        *chosenRadiusType = toolBox->getComponent<QAbstractButton*>("mm")->isChecked() ? RADIUS_IN_MM : RADIUS_IN_PIXELS;
    }

    *output = getFilteredImaged();
}

PipelineStepProperties FilteringDelegate::createDelegateSizeThresholdingStep(QString input, QString output,
                                                                             QString thresholdOutput,
                                                                             RetryMode retryMode)
{
    return createSizeThresholdingStep("SizeThresholding")
            .setDelegateClass(staticMetaObject.className())
            .requires(input)
            .setOption("input", input)
            .setOption("output", output)
            .setOption("thresholdOutput", thresholdOutput)
            .setOption<int>("retryMode", retryMode);
}

void FilteringDelegate::startSizeThresholdingStep(PipelineWorkingState* state)
{
    dtkSmartPointer<medAbstractData> input = state->getValue(state->getStepOption<QString>("input"));
    RetryMode retryMode = static_cast<RetryMode>(state->getStepOption<int>("retryMode"));
    startSizeThresholdingStep(input, retryMode);
}

void FilteringDelegate::endSizeThresholdingStep(PipelineWorkingState* state)
{
    dtkSmartPointer<medAbstractData> output;
    double chosenThreshold;
    endSizeThresholdingStep(&output, &chosenThreshold);
    QString outputId = state->getStepOption<QString>("output");
    state->setValue(outputId, output);
    state->copyWindowLevel(outputId, state->getStepOption<QString>("input"));
    QString thresholdOutput = state->getStepOption<QString>("thresholdOutput");
    if (thresholdOutput != "")
    {
        state->setValue(thresholdOutput, chosenThreshold);
    }
}

PipelineStepProperties FilteringDelegate::createDelegateThresholdingStep(QString input, QString output,
                                                                                       double threshold, int outsideValue,
                                                                                       ThresholdType thresholdType, bool showHistogram,
                                                                                       double lowerValue, double upperValue)
{
    return createThresholdingStep("Thresholding")
            .setDelegateClass(staticMetaObject.className())
            .requires(input)
            .setOption<QString>("input", input)
            .setOption<QString>("output", output)
            .setOption<double>("lowerValue", lowerValue)
            .setOption<double>("upperValue", upperValue)
            .setOption<double>("thresholdValue", threshold)
            .setOption<int>("outsideThresholdValue", outsideValue)
            .setOption<int>("thresholdType", thresholdType)
            .setOption<bool>("showHistogram", showHistogram);
}

void FilteringDelegate::startThresholdingStep(PipelineWorkingState *state)
{
    dtkSmartPointer<medAbstractData> input = state->getValue(state->getStepOption<QString>("input"));
    ThresholdType thresholdType = static_cast<ThresholdType>(state->getStepOption<int>("thresholdType"));
    double lowerValue = state->getStepOption<double>("lowerValue");
    double upperValue = state->getStepOption<double>("upperValue");
    double thresholdValue = state->getStepOption<double>("thresholdValue");
    int outsideValue = state->getStepOption<int>("outsideThresholdValue");
    bool showHistogram = state->getStepOption<bool>("showHistogram");
    startThresholdingStep(input, thresholdValue, outsideValue, thresholdType, showHistogram, lowerValue, upperValue);
}

void FilteringDelegate::endThresholdingStep(PipelineWorkingState *state)
{
    dtkSmartPointer<medAbstractData> output;
    endThresholdingStep(&output);
    QString outputId = state->getStepOption<QString>("output");
    state->setValue(outputId, output);
    state->copyWindowLevel(outputId, state->getStepOption<QString>("input"));
}

PipelineStepProperties FilteringDelegate::createDelegateBinaryThresholdingStep(QString input, QString output,
                                                                                             int insideValue, int outsideValue,
                                                                                             bool showHistogram,
                                                                                             double lowerValue, double upperValue)
{
    return createBinaryThresholdingStep("BinaryThresholding")
            .setDelegateClass(staticMetaObject.className())
            .requires(input)
            .setOption<QString>("input", input)
            .setOption<QString>("output", output)
            .setOption<double>("lowerValue", lowerValue)
            .setOption<double>("upperValue", upperValue)
            .setOption<int>("insideValue", insideValue)
            .setOption<int>("outsideValue", outsideValue)
            .setOption<bool>("showHistogram", showHistogram);
}

void FilteringDelegate::startBinaryThresholdingStep(PipelineWorkingState *state)
{
    dtkSmartPointer<medAbstractData> input = state->getValue(state->getStepOption<QString>("input"));
    RetryMode retryMode = static_cast<RetryMode>(state->getStepOption<int>("retryMode"));
    double lowerValue = state->getStepOption<double>("lowerValue");
    double upperValue = state->getStepOption<double>("upperValue");
    int insideValue = state->getStepOption<double>("insideValue");
    int outsideValue = state->getStepOption<int>("outsideValue");
    bool showHistogram = state->getStepOption<bool>("showHistogram");
    startBinaryThresholdingStep(input, insideValue, outsideValue, showHistogram, lowerValue, upperValue);
}

void FilteringDelegate::endBinaryThresholdingStep(PipelineWorkingState *state)
{
    dtkSmartPointer<medAbstractData> output;
    endBinaryThresholdingStep(&output);
    QString outputId = state->getStepOption<QString>("output");
    state->setValue(outputId, output);
    state->copyWindowLevel(outputId, state->getStepOption<QString>("input"));
}

PipelineStepProperties FilteringDelegate::createDelegateMorphologicalFilteringStep(QString input, QString output,
                                                                                                 MorphologicalFilterType filterType,
                                                                                                 RetryMode retryMode,
                                                                                                 RadiusType radiusTypes,
                                                                                                 int initialRadius)
{
    return createMorphologicalFilteringStep("MorphologicalFiltering", filterType)
            .setDelegateClass(staticMetaObject.className())
            .requires(input)
            .setOption("input", input)
            .setOption("output", output)
            .setOption<int>("filterType", filterType)
            .setOption<int>("retryMode", retryMode)
            .setOption<int>("radiusType", radiusTypes)
            .setOption<int>("initialRadius", initialRadius);
}

void FilteringDelegate::startMorphologicalFilteringStep(PipelineWorkingState* state)
{
    dtkSmartPointer<medAbstractData> input = state->getValue(state->getStepOption<QString>("input"));
    MorphologicalFilterType filterType = static_cast<MorphologicalFilterType>(state->getStepOption<int>("filterType"));
    RetryMode retryMode = static_cast<RetryMode>(state->getStepOption<int>("retryMode"));
    RadiusType radiusType = static_cast<RadiusType>(state->getStepOption<int>("radiusType"));
    int intialRadius = state->getStepOption<int>("initialRadius");
    startMorphologicalFilteringStep(input, filterType, retryMode, radiusType, intialRadius);
}

void FilteringDelegate::endMorphologicalFilteringStep(PipelineWorkingState* state)
{
    dtkSmartPointer<medAbstractData> output;
    endMorphologicalFilteringStep(&output);
    QString outputId = state->getStepOption<QString>("output");
    state->setValue(outputId, output);
    state->copyWindowLevel(outputId, state->getStepOption<QString>("input"));
}

void FilteringDelegate::startFilteringStep(dtkSmartPointer<medAbstractData> input, int filterType, RetryMode retryMode)
{
    d->filterType = filterType;
    d->input = input;
    d->retryMode = retryMode;

    QString toolBoxIdentifier = getFilterToolBoxIdentifier(filterType);
    medAbstractSelectableToolBox* toolBox = static_cast<medAbstractSelectableToolBox*>(getWorkspace()->addToolBox(toolBoxIdentifier));
    getWorkspace()->addData(input);

    setupViewParameterGroup(filterNames[filterType]);
    setupSelectorToolBox(toolBoxIdentifier, input);
    toolBox->setTitle(filterNames[filterType]);
    toolBox->setSelectorToolBox(d->selectorToolBox.data());

    QComboBox* filtersComboBox = toolBox->getComponent<QComboBox*>(getFilterComboBoxName(filterType));
    filtersComboBox->setCurrentIndex(getFilterComboBoxIndex(filterType));
    filtersComboBox->hide();     //cannot be hidden before changing index

    if(d->retryMode == MULTIPLE_TRIES)
    {
        getWorkspace()->getToolBox<PipelineControlToolBox>()->disableValidation();

        if (isBasicFilter(filterType))
        {
            connect(toolBox, SIGNAL(success()), this, SLOT(displayBasicFilteringResult()));
        }
        else if (isMorphologicalFilter(filterType))
        {
            connect(toolBox, SIGNAL(success()), this, SLOT(displayMorphologicalResult()));
        }
    }
}

void FilteringDelegate::setupViewParameterGroup(QString name)
{
    d->viewGroup = QSharedPointer<medViewParameterGroupL>(new medViewParameterGroupL(name));
    d->viewGroup->setLinkAllParameters(true);
    //Little hack: linkAll() = true, creates pb in medViewParameterGroup::updateParameterToLinkList
    d->viewGroup->removeParameter("All");
}

void FilteringDelegate::setupSelectorToolBox(QString toolBoxIdentifier, dtkSmartPointer<medAbstractData> input)
{
    d->selectorToolBox = QSharedPointer<medSelectorToolBox>(new medSelectorToolBox(nullptr, "Filtering"));
    d->selectorToolBox->setWorkspace(getWorkspace());
    d->selectorToolBox->changeCurrentToolBox(toolBoxIdentifier);
    d->selectorToolBox->onInputSelected(input);
}

dtkSmartPointer<medAbstractData> FilteringDelegate::getFilteredImaged()
{
    QString toolBoxIdentifier = getFilterToolBoxIdentifier(d->filterType);

    if (d->retryMode == SINGLE_TRY)
    {
        medToolBox* toolBox = getWorkspace()->getToolBox(toolBoxIdentifier);
        toolBox->getComponent<QAbstractButton*>("Run")->click();
        QEventLoop loop;
        connect(toolBox, SIGNAL(success()), &loop, SLOT(quit()));
        connect(toolBox, SIGNAL(failure()), &loop, SLOT(quit()));
        loop.exec();
    }

    dtkSmartPointer<medAbstractData> output = getWorkspace()->getToolBoxOutput(toolBoxIdentifier);
    medUtilities::setDerivedMetaData(output, d->input, "");
    return output;
}

QString FilteringDelegate::getFilterToolBoxIdentifier(int filterType)
{
    if (isBasicFilter(filterType))
    {
        return ITK_FILTERS_TOOLBOX;
    }
    else if (isMorphologicalFilter(filterType))
    {
        return ITK_MORPHOLOGICAL_FILTERS_TOOLBOX;
    }
    else
    {
        THROW_METHOD_EXCEPTION("Invalid filter type: " + QString::number(filterType));
    }
}

QString FilteringDelegate::getFilterComboBoxName(int filterType)
{
    if (isBasicFilter(filterType))
    {
        return "Add constant";
    }
    else if (isMorphologicalFilter(filterType))
    {
        return "Dilate";
    }
    return "";
}

int FilteringDelegate::getFilterComboBoxIndex(int filterType)
{
    if (isBasicFilter(filterType))
    {
        return filterType - MIN_BASIC_FILTER_TYPE;
    }
    else if (isMorphologicalFilter(filterType))
    {
        return filterType - (MAX_BASIC_FILTER_TYPE + 1);
    }
    return -1;
}

bool FilteringDelegate::isBasicFilter(int filterType)
{
    return filterType <= MAX_BASIC_FILTER_TYPE;
}

bool FilteringDelegate::isMorphologicalFilter(int filterType)
{
    return filterType > MAX_BASIC_FILTER_TYPE;
}

void FilteringDelegate::displayBasicFilteringResult()
{
    dtkSmartPointer<medAbstractData> output = getWorkspace()->getToolBoxOutput("itkFiltersToolBox");
    medViewContainer* newContainer;

    if (getWorkspace()->tabbedViewContainers()->containersInTab(0).count() == 1)
    {
        newContainer = getWorkspace()->getContainer(0)->splitVertically();
    }
    else
    {
        newContainer = getWorkspace()->getContainer(1)->splitHorizontally();
    }

    newContainer->addData(output);
    d->viewGroup->addImpactedView(getWorkspace()->getContainer()->view());
    d->viewGroup->addImpactedView(newContainer->view());

    if(d->filterType == THRESHOLDING)
    {
        return;
    }
    if (getWorkspace()->getToolBox(ITK_FILTERS_TOOLBOX)->getValue<int>("componentSizeThresholdFilterValue") > 0)
    {
        getWorkspace()->getToolBox<PipelineControlToolBox>()->enableValidation();
    }
     getWorkspace()->setVolumeRendering(getWorkspace()->tabbedViewContainers()->containersInTab(0).indexOf(newContainer));
}

void FilteringDelegate::displayMorphologicalResult()
{
    dtkSmartPointer<medAbstractData> output = getWorkspace()->getToolBoxOutput("itkMorphologicalFiltersToolBox");
    medViewContainer* newContainer = getWorkspace()->getContainer()->splitVertically();
    newContainer->addData(output);
    d->viewGroup->addImpactedView(getWorkspace()->getContainer()->view());
    d->viewGroup->addImpactedView(newContainer->view());

    getWorkspace()->getToolBox<PipelineControlToolBox>()->enableValidation();
}

} // namespace med::pipeline

