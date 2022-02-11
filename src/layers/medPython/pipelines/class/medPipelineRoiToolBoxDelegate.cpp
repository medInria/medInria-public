#include "medPipelineRoiToolBoxDelegate.h"

#include <itkImageDuplicator.h>

#include <dtkCoreSupport/dtkAbstractProcessFactory>

#include <medAbstractProcessLegacy.h>
#include <medAbstractDataFactory.h>
#include <medDataManager.h>
#include <medDoubleParameterL.h>
#include <medTabbedViewContainers.h>
#include <medUtilities.h>
#include <medViewContainer.h>

#include "medPipelineControlToolBox.h"
#include "medPipelineFactory.h"
#include "medPipelineTypes.h"
#include "medPipelineWorkspace.h"

#if defined ( WIN32 )
#define __func__ __FUNCTION__
#endif

namespace med::pipeline
{

class RoiToolboxDelegatePrivate
{
public:
    dtkSmartPointer<medAbstractData> input;
    QList<dtkSmartPointer<medAbstractData> > paintMasks;
    RetryMode retryMode;
};

void RoiToolboxDelegate::registerDelegate()
{
    PipelineFactory::instance()->registerDelegate<RoiToolboxDelegate>();
}

RoiToolboxDelegate::RoiToolboxDelegate(void* argument)
    : d(new RoiToolboxDelegatePrivate())
{
}

RoiToolboxDelegate::~RoiToolboxDelegate()
{
    delete d;
}

PipelineStepProperties RoiToolboxDelegate::createRoiToolboxStep(QString name)
{
    return PipelineStepProperties(name)
            .setTitle("ROI Tools")
            .setDescription("Choose the method to perform the segmentation");
}

void RoiToolboxDelegate::startRoiToolboxStep(dtkSmartPointer<medAbstractData> input)
{
    d->input = input;

    Q_UNUSED(getWorkspace()->addToolBox(ROI_TOOLBOX));
    getWorkspace()->addData(input);
    getWorkspace()->setToolBoxVisibility(MOUSE_INTERACTION_TOOLBOX, false);
    getWorkspace()->setToolBoxVisibility(VIEW_SETTINGS_TOOLBOX, false);
    getWorkspace()->setToolBoxVisibility(LAYERS_TOOLBOX, false);
}

void RoiToolboxDelegate::endRoiToolboxStep(dtkSmartPointer<medAbstractData> *output, OutputType outputType)
{
    *output = convertToOutputType(getWorkspace()->getDroppedResultOrToolBoxOutput("medPipelineRoiToolBox"), outputType, d->input);
}

PipelineStepProperties RoiToolboxDelegate::createDelegateRoiToolboxStep(QString input, QString output,
                                                                                      OutputType outputType)
{
    return createRoiToolboxStep("RoiToolbox")
            .setDelegateClass(staticMetaObject.className())
            .requires(input)
            .setOption("input", input)
            .setOption("output", output)
            .setOption<int>("outputType", outputType);
}

void RoiToolboxDelegate::startRoiToolboxStep(PipelineWorkingState *state)
{
    dtkSmartPointer<medAbstractData> input = state->getValue(state->getStepOption<QString>("input"));
    startRoiToolboxStep(input);
}

void RoiToolboxDelegate::endRoiToolboxStep(PipelineWorkingState *state)
{
    dtkSmartPointer<medAbstractData> output;
    OutputType outputType = static_cast<OutputType>(state->getStepOption<int>("outputType"));
    endRoiToolboxStep(&output, outputType);
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
        state->reject("Error during Segmentation method selection.");
    }

}

dtkSmartPointer<medAbstractData> RoiToolboxDelegate::convertToOutputType(dtkSmartPointer<medAbstractData> mask,
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
            throw std::invalid_argument("RoiToolboxDelegate::convertToOutputType wrong type given.");
        }
        }
    }
    return res;
}

} // namespace med::pipeline
