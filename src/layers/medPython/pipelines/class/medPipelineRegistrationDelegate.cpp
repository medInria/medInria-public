#include "medPipelineRegistrationDelegate.h"

#include "medPipelineFactory.h"
#include <medTabbedViewContainers.h>
#include <medToolBox.h>

namespace med::pipeline
{

class RegistrationDelegatePrivate
{
public:
};

void RegistrationDelegate::registerDelegate()
{
    PipelineFactory::instance()->registerDelegate<RegistrationDelegate>();
}

RegistrationDelegate::RegistrationDelegate(void* argument)
    : d(new RegistrationDelegatePrivate())
{
}

RegistrationDelegate::~RegistrationDelegate()
{
    delete d;
}

PipelineStepProperties RegistrationDelegate::createManualRegistrationStep(QString name)
{
    return PipelineStepProperties(name)
            .setTitle("Registration")
            .setDescription("Apply registration on both volumes.\n\tUse 'shift-click' to add a landmark.\n\tUse 'ctrl-click' to remove a landmark.\n\tLandmark numbers have to be the same.")
            .setResultDropMode(SINGLE_RESULT_DROP);
}

void RegistrationDelegate::startManualRegistrationStep(dtkSmartPointer<medAbstractData> fixed, dtkSmartPointer<medAbstractData> moving)
{
    medToolBox* toolBox = getWorkspace()->addToolBox(MANUAL_REGISTRATION_TOOLBOX);

    getWorkspace()->addData(fixed);
    getWorkspace()->addData(moving);

    toolBox->getComponent<QAbstractButton*>("startManualRegistrationButton")->click();

    toolBox->getComponent<QAbstractButton*>("startManualRegistrationButton")->hide();
    toolBox->getComponent<QAbstractButton*>("resetButton")->hide();
    getWorkspace()->tabbedViewContainers()->closeTab(1);
    getWorkspace()->setToolBoxVisibility(MOUSE_INTERACTION_TOOLBOX, false);
    getWorkspace()->setToolBoxVisibility(VIEW_SETTINGS_TOOLBOX, false);
    getWorkspace()->setToolBoxVisibility(LAYERS_TOOLBOX, false);
}

void RegistrationDelegate::endManualRegistrationStep(dtkSmartPointer<medAbstractData>* output)
{
    medToolBox* toolBox = getWorkspace()->getToolBox(MANUAL_REGISTRATION_TOOLBOX);
    *output = getWorkspace()->getDroppedResultOrToolBoxOutput(MANUAL_REGISTRATION_TOOLBOX);
    toolBox->getComponent<QAbstractButton*>("startManualRegistrationButton")->click();
}

PipelineStepProperties RegistrationDelegate::createDelegateManualRegistrationStep(QString fixed, QString moving, QString output)
{
    return createManualRegistrationStep("ManualRegistration")
            .setDelegateClass(staticMetaObject.className())
            .requires(fixed)
            .requires(moving)
            .setOption("fixed", fixed)
            .setOption("moving", moving)
            .setOption("output", output);
}

void RegistrationDelegate::startManualRegistrationStep(PipelineWorkingState* state)
{
    startManualRegistrationStep(state->getValue(state->getStepOption<QString>("fixed")),
                                state->getValue(state->getStepOption<QString>("moving")));
}

void RegistrationDelegate::endManualRegistrationStep(PipelineWorkingState* state)
{
    dtkSmartPointer<medAbstractData> output;
    endManualRegistrationStep(&output);
    if (output)
    {
        QString outputId = state->getStepOption<QString>("output");
        state->setValue(outputId, output);
        state->copyWindowLevel(outputId, state->getStepOption<QString>("moving"));
    }
    else
    {
        state->reject("Error during registration.");
    }
}

} // namespace med::pipeline
