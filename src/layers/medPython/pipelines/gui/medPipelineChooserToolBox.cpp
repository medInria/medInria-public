#include "medPipelineChooserToolBox.h"

#include <stdexcept>

#include <QVBoxLayout>

#include <medAbstractLayeredView.h>
#include <medDataManager.h>
#include <medDropSite.h>
#include <medMessageController.h>
#include <medPluginManager.h>
#include <medSettingsManager.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medViewContainer.h>

#include "medPipelineFactory.h"
#include "medPipelineInstance.h"
#include "medPipelineWorkspace.h"
#include "medPythonPipeline.h"
#include "medPythonPipelineInstance.h"

namespace med::pipeline
{

class PipelineChooserToolBoxPrivate
{
public:
    QWidget* mainWidget;
    QWidget* pipelineButtons;
};

bool PipelineChooserToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<PipelineChooserToolBox>();
}

PipelineChooserToolBox::PipelineChooserToolBox(QWidget* parent) : medToolBox(parent), d(new PipelineChooserToolBoxPrivate())
{
    d->mainWidget = new QWidget();
    addWidget(d->mainWidget);
    d->mainWidget->setLayout(new QVBoxLayout());

    QLabel* label = new QLabel(tr("Available pipelines:"));
    label->setAlignment(Qt::AlignCenter);
    d->mainWidget->layout()->addWidget(label);

    createLaunchButtonsForPipelines();
}

PipelineChooserToolBox::~PipelineChooserToolBox()
{
    delete d;
}

void PipelineChooserToolBox::createDropContainer()
{
    medTabbedViewContainers* tabs = PipelineWorkspace::instance()->tabbedViewContainers();
    medViewContainer* viewContainer = tabs->addContainerInTabNamed("Pipeline drop");
    viewContainer->setMultiLayered(false);
    viewContainer->setUserSplittable(false);
    viewContainer->setUserOpenable(false);
    connect(viewContainer, SIGNAL(containerSelected(QUuid)), this, SLOT(dataDropped()));

    medDropSite* dropSite = new medDropSite();
    dropSite->setStyleSheet("background-color: rgba(255, 255, 255, 0);");
    dropSite->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    dropSite->setText("To create a new pipeline, use the buttons on the right.\nTo load a previous pipeline, drag'n drop its #MAIN file here.");
    dropSite->acceptDrops();
    connect(dropSite, SIGNAL(objectDropped(medDataIndex)), this, SLOT(continueExistingPipeline(medDataIndex)));
    viewContainer->setDefaultWidget(dropSite);
}

void PipelineChooserToolBox::startNewPipeline(QString identifier)
{
    try
    {
        AbstractPipeline* pipeline = PipelineFactory::instance()->createPipeline(identifier);
        PythonPipeline* pythonPipeline = dynamic_cast<PythonPipeline*>(pipeline);
        dtkSmartPointer<PipelineInstance> instance;

        if (pythonPipeline != nullptr)
        {
            instance = new PythonPipelineInstance(pythonPipeline);
        }
        else
        {
            instance = new PipelineInstance(pipeline);
        }

        runPipeline(instance);
    }
    catch (std::exception& e)
    {
        PipelineWorkspace::instance()->stopPipeline();
        PipelineWorkspace::instance()->displayException("Error while starting pipeline:", e);
    }
}

void PipelineChooserToolBox::continueExistingPipeline(const medDataIndex& index)
{
    try
    {
        dtkSmartPointer<PipelineInstance> instance = dynamic_cast<PipelineInstance*>(medDataManager::instance()->retrieveData(index));

        if (instance.data() == nullptr)
        {
            throw std::runtime_error("Pipeline instance could not be retrieved");
        }

        runPipeline(instance);
    }
    catch (std::exception& e)
    {
        PipelineWorkspace::instance()->stopPipeline();
        PipelineWorkspace::instance()->displayException("Error while loading pipeline:", e);
    }
}

void PipelineChooserToolBox::dataDropped()
{
    medViewContainer* viewContainer = PipelineWorkspace::instance()->getContainer();
    medAbstractLayeredView* layeredView = qobject_cast<medAbstractLayeredView*>(viewContainer->view());
    dtkSmartPointer<medAbstractData> data = layeredView->layerData(0);
    viewContainer->close();
    continueExistingPipeline(data->dataIndex());
}

void PipelineChooserToolBox::createLaunchButtonsForPipelines()
{
    d->pipelineButtons = new QWidget();
    d->pipelineButtons->setLayout(new QVBoxLayout());
    d->mainWidget->layout()->addWidget(d->pipelineButtons);


//    foreach (QUuid identifier, PipelineFactory::instance()->getRegisteredPipelines())
//    {
//        QString title = PipelineFactory::instance()->getPipelineTitle(identifier);
//        QPushButton* button = new QPushButton(title);
//        connect(button, &QPushButton::clicked, [=] { startNewPipeline(identifier); });
//        d->pipelineButtons->layout()->addWidget(button);
//    }
}

void PipelineChooserToolBox::runPipeline(dtkSmartPointer<PipelineInstance> pipeline)
{
    PipelineWorkspace::instance()->runPipeline(pipeline);
}

} // namespace med::pipeline
