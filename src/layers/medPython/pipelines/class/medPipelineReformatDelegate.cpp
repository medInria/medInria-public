#include "medPipelineReformatDelegate.h"

#include <medAbstractLayeredView.h>
#include <medToolBox.h>
#include <medToolBoxBody.h>
#include <medViewContainer.h>

#include "medPipelineFactory.h"
#include "medPipelineWorkspace.h"

namespace med::pipeline
{

class ReformatDelegatePrivate
{
public:
};

void ReformatDelegate::registerDelegate()
{
    PipelineFactory::instance()->registerDelegate<ReformatDelegate>();
}

ReformatDelegate::ReformatDelegate(void* argument)
    : d(new ReformatDelegatePrivate())
{
}

ReformatDelegate::~ReformatDelegate()
{
    delete d;
}

PipelineStepProperties ReformatDelegate::createResliceStep(QString name)
{
    return PipelineStepProperties(name)
            .setTitle("Reslice")
            .setDescription("Reslice the volume.\n\t'o' to reset orientation.\n\t'r' to reset current windowing.\n\t'left click' on 2D views to change windowing.")
            .setResultDropMode(SINGLE_RESULT_DROP);
}

void ReformatDelegate::startResliceStep(dtkSmartPointer<medAbstractData> input, QList<double> spacing)
{
    medToolBox* toolBox = getWorkspace()->addToolBox(RESLICE_TOOLBOX);
    toolBox->getComponent<QAbstractButton*>("saveImageButton")->hide();
    toolBox->getComponent<QAbstractButton*>("startReformatButton")->hide();
    toolBox->getComponent<QAbstractButton*>("stopReformatButton")->hide();

    getWorkspace()->addData(input);

    toolBox->getComponent<QAbstractButton*>("startReformatButton")->click();

    if (spacing.length() == 3)
    {
        toolBox->setValue("SpacingX", spacing[0]);
        toolBox->setValue("SpacingY", spacing[1]);
        toolBox->setValue("SpacingZ", spacing[2]);
    }

    getWorkspace()->setToolBoxVisibility(MOUSE_INTERACTION_TOOLBOX, false);
    getWorkspace()->setToolBoxVisibility(VIEW_SETTINGS_TOOLBOX, false);
    getWorkspace()->setToolBoxVisibility(LAYERS_TOOLBOX, false);
}

void ReformatDelegate::endResliceStep(dtkSmartPointer<medAbstractData>* output)
{
    *output = getWorkspace()->getDroppedResultOrToolBoxOutput(RESLICE_TOOLBOX);
}

PipelineStepProperties ReformatDelegate::createFrameExtractionStep(QString name)
{
    return PipelineStepProperties(name)
            .setTitle("Frame Extraction")
            .setDescription("Extract frame: select a frame with the slider of the 'View settings' toolbox then click Next.")
            .setResultDropMode(SINGLE_RESULT_DROP);
}

void ReformatDelegate::startFrameExtractionStep(dtkSmartPointer<medAbstractData> input)
{
    medToolBox* toolBox = getWorkspace()->addToolBox(EXTRACTION_4D_TO_3D_TOOLBOX);
    toolBox->setFixedHeight(0);
    getWorkspace()->addData(input);
}

void ReformatDelegate::endFrameExtractionStep(dtkSmartPointer<medAbstractData> *output)
{
    *output = getWorkspace()->getDroppedResultOrToolBoxOutput(EXTRACTION_4D_TO_3D_TOOLBOX);
}

PipelineStepProperties ReformatDelegate::createCroppingStep(QString name)
{
    return PipelineStepProperties(name)
            .setTitle("Cropping")
            .setDescription("Crop the volume.");
}

void ReformatDelegate::startCroppingStep(dtkSmartPointer<medAbstractData> input)
{
    QList<dtkSmartPointer<medAbstractData> > inputs;
    inputs.append(input);
    startCroppingStep(inputs);
}

void ReformatDelegate::startCroppingStep(QList<dtkSmartPointer<medAbstractData> > inputs)
{
    medToolBox* toolBox = getWorkspace()->addToolBox(CROP_TOOLBOX);
    toolBox->getComponent<QAbstractButton*>("Apply")->hide();
    toolBox->getComponent<QAbstractButton*>("Save")->hide();

    foreach (dtkSmartPointer<medAbstractData> input, inputs)
    {
        getWorkspace()->addData(input);
    }

    if (toolBox->body()->isHidden())
    {
        toolBox->switchMinimize();
    }

    toolBox->setFixedHeight(0);
}

void ReformatDelegate::endCroppingStep(dtkSmartPointer<medAbstractData>* output)
{
    QList<dtkSmartPointer<medAbstractData> > outputs;
    endCroppingStep(&outputs);
    *output = outputs.first();
}

void ReformatDelegate::endCroppingStep(QList<dtkSmartPointer<medAbstractData> >* outputs)
{
    getWorkspace()->getToolBox(CROP_TOOLBOX)->getComponent<QAbstractButton*>("Apply")->click();

    medAbstractLayeredView* view = static_cast<medAbstractLayeredView*>(getWorkspace()->getContainer()->view());

    for (unsigned int i = 0; i < view->layersCount(); i++)
    {
       outputs->append(getWorkspace()->getLayerData(i));
    }
}

PipelineStepProperties ReformatDelegate::createDelegateResliceStep(QString input, QString output,
                                                                   QList<double> spacing)
{
    return createResliceStep("Reslice")
            .setDelegateClass(staticMetaObject.className())
            .requires(input)
            .setOption("input", input)
            .setOption("output", output)
            .setOption("spacing", spacing);
}

void ReformatDelegate::startResliceStep(PipelineWorkingState* state)
{
    startResliceStep(state->getValue(state->getStepOption<QString>("input")), state->getStepOption<QList<double> >("spacing"));
}

void ReformatDelegate::endResliceStep(PipelineWorkingState* state)
{
    dtkSmartPointer<medAbstractData> output;
    endResliceStep(&output);
    QString outputId = state->getStepOption<QString>("output");
    state->setValue(outputId, output);
    state->copyWindowLevel(outputId, state->getStepOption<QString>("input"));
}

PipelineStepProperties ReformatDelegate::createDelegateFrameExtractionStep(QString input, QString output)
{
    return createFrameExtractionStep("FrameExtraction")
            .setDelegateClass(staticMetaObject.className())
            .requires(input)
            .setOption("input", input)
            .setOption("output", output);
}

void ReformatDelegate::startFrameExtractionStep(PipelineWorkingState* state)
{
    startFrameExtractionStep(state->getValue(state->getStepOption<QString>("input")));
}

void ReformatDelegate::endFrameExtractionStep(PipelineWorkingState* state)
{
    dtkSmartPointer<medAbstractData> output;
    endFrameExtractionStep(&output);

    if(output)
    {
        QString outputId = state->getStepOption<QString>("output");
        state->setValue(outputId, output);
        state->copyWindowLevel(outputId, state->getStepOption<QString>("input"));
    }
    else
    {
        state->reject("Error during frame extraction. Check that your input is a 4D sequence or skip the step.");
    }
}

PipelineStepProperties ReformatDelegate::createDelegateCroppingStep(QStringList inputs, QStringList outputs)
{
    return createCroppingStep("Cropping")
            .setDelegateClass(staticMetaObject.className())
            .requiresOneOf(inputs)
            .setOption("inputs", inputs)
            .setOption("outputs", outputs);
}

void ReformatDelegate::startCroppingStep(PipelineWorkingState* state)
{
    QList<dtkSmartPointer<medAbstractData> > inputs;

    foreach (QString input, state->getStepOption<QStringList>("inputs"))
    {
        dtkSmartPointer<medAbstractData> data = state->getValue(input);
        if (data)
        {
            inputs.append(data);
        }
    }

    startCroppingStep(inputs);
}

void ReformatDelegate::endCroppingStep(PipelineWorkingState* state)
{
    QList<dtkSmartPointer<medAbstractData> > outputValues;
    endCroppingStep(&outputValues);
    QStringList inputs = state->getStepOption<QStringList>("inputs");
    QStringList outputs = state->getStepOption<QStringList>("outputs");

    for (int i = 0; i < inputs.length(); i++)
    {
        if (state->getValue(inputs[i]))
        {
            state->setValue(outputs[i], outputValues[i]);
            state->copyWindowLevel(outputs[i], inputs[i]);
        }
    }
}

PipelineStepProperties ReformatDelegate::createMergeVolumesStep(QString name)
{
    return PipelineStepProperties(name)
            .setTitle("Merge Volumes")
            .setDescription("Merge Volumes: drop a batch of volumes in the View and they will be merged after clicking on Next.")
            .setResultDropMode(SINGLE_RESULT_DROP);
}
void ReformatDelegate::startMergeVolumesStep(dtkSmartPointer<medAbstractData> input)
{
    medToolBox* toolBox = getWorkspace()->addToolBox(MERGE_VOLUMES_TOOLBOX);
    toolBox->getComponent<QAbstractButton*>("Run")->hide();
    getWorkspace()->addData(input);
}

void ReformatDelegate::endMergeVolumesStep(dtkSmartPointer<medAbstractData>* output)
{
    *output = getWorkspace()->getDroppedResultOrToolBoxOutput(MERGE_VOLUMES_TOOLBOX);
}

PipelineStepProperties ReformatDelegate::createDelegateMergeVolumesStep(QString input, QString output)
{
    return createMergeVolumesStep("MergeVolumes")
            .setDelegateClass(staticMetaObject.className())
            .requires(input)
            .setOption("input", input)
            .setOption("output", output);
}

void ReformatDelegate::startMergeVolumesStep(PipelineWorkingState* state)
{
    startMergeVolumesStep(state->getValue(state->getStepOption<QString>("input")));
}

void ReformatDelegate::endMergeVolumesStep(PipelineWorkingState* state)
{
    dtkSmartPointer<medAbstractData> output;
    endMergeVolumesStep(&output);

    if(output)
    {
        QString outputId = state->getStepOption<QString>("output");
        state->setValue(outputId, output);
        state->copyWindowLevel(outputId, state->getStepOption<QString>("input"));
    }
    else
    {
        state->reject("You need to drop a batch of volumes in the View in order to proceed.");
    }

}

} // namespace med::pipeline
