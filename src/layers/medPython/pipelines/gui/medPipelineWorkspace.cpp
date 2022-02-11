#include "medPipelineWorkspace.h"

#include <vtkImageView2D.h>
#include <vtkLookupTableManager.h>

#include <medAbstractImageView.h>
#include <medAbstractImageViewInteractor.h>
#include <medAbstractLayeredView.h>
#include <medAbstractParameterL.h>
#include <medAbstractSelectableToolBox.h>
#include <medDataManager.h>
#include <medDoubleParameterL.h>
#include <medMessageController.h>
#include <medRegistrationAbstractToolBox.h>
#include <medStringListParameterL.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxBody.h>
#include <medToolBoxFactory.h>
#include <medToolBoxHeader.h>
#include <medUtilities.h>
#include <medViewContainer.h>
#include <medViewContainerManager.h>
#include <medViewEventFilter.h>
#include <medVtkViewBackend.h>
#include <medWorkspaceFactory.h>

#include "medPipelineAttachedData.h"
#include "medPipelineChooserToolBox.h"
#include "medPipelineControlToolBox.h"

namespace med::pipeline
{

class WorkspacePrivate
{
public:
    static PipelineWorkspace* instance;
    QList<medToolBox*> fixedToolBoxes;
    bool isCurrentlyRunningPipeline;
    QMutex importMutex;
    dtkSmartPointer<medAbstractData> currentImport;
    medDataIndex currentImportIndex;
    QUuid currentImportId;

    WorkspacePrivate() : importMutex(QMutex::Recursive) {}
};

PipelineWorkspace* WorkspacePrivate::instance = nullptr;

bool PipelineWorkspace::isUsable()
{
    return true;
}

bool PipelineWorkspace::registered()
{
    return medWorkspaceFactory::instance()->registerWorkspace<PipelineWorkspace>();
}

void PipelineWorkspace::displayError(QString title, QString message)
{
    qDebug() << title;
    qDebug() << message;

    medMessageController::instance()->showError(title, 3000);

    QMessageBox messsageBox;
    QFontMetrics fontMetrics(messsageBox.font());
    int width = fontMetrics.boundingRect(message).width();
    QSpacerItem* horizontalSpacer = new QSpacerItem(width, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout* layout = static_cast<QGridLayout*>(messsageBox.layout());
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
    messsageBox.setText(title);
    messsageBox.setInformativeText(message);
    messsageBox.setStandardButtons(QMessageBox::Ok);
    messsageBox.exec();
}

PipelineWorkspace* PipelineWorkspace::instance()
{
    return WorkspacePrivate::instance;
}

PipelineWorkspace::PipelineWorkspace(QWidget *parent)
    : medAbstractWorkspaceLegacy(parent), d(new WorkspacePrivate())
{
    d->instance = this;
    d->isCurrentlyRunningPipeline = false;

    connect(this, SIGNAL(exceptionAnnounced(QString, QString)),
            this, SLOT(displayExceptionBox(QString, QString)));

    d->fixedToolBoxes.append(getMouseInteractionToolBox());
    d->fixedToolBoxes.append(getNavigatorToolBox());
    d->fixedToolBoxes.append(getLayersToolBox());
    d->fixedToolBoxes.append(getProgressionStackToolBox());
    d->fixedToolBoxes.append(insertToolBox<PipelineChooserToolBox>(0));
    d->fixedToolBoxes.append(insertToolBox<PipelineControlToolBox>(0));

    connect(medDataManager::instance(), SIGNAL(dataRemoved(medDataIndex)),
            this, SLOT(importCurrentData(medDataIndex)));
    connect(medDataManager::instance(), SIGNAL(dataImported(medDataIndex, QUuid)),
            this, SLOT(updateImportedDataIndex(medDataIndex, QUuid)));

    clear();
}

PipelineWorkspace::~PipelineWorkspace()
{  
    delete d;
}

void PipelineWorkspace::setupTabbedViewContainer()
{
    medTabbedViewContainers* tabs = tabbedViewContainers();
    tabs->setKeepLeastOne(false);

    while (tabs->count() > 0)
    {
        tabs->closeCurrentTab();
    }

    getToolBox<PipelineChooserToolBox>()->createDropContainer();
}

void PipelineWorkspace::clear()
{
    foreach (medToolBox* toolBox, toolBoxes())
    {
        if (!d->fixedToolBoxes.contains(toolBox))
        {
            removeToolBox(toolBox);
        }
    }

    setToolBoxVisibility(MOUSE_INTERACTION_TOOLBOX, false);
    setToolBoxVisibility(VIEW_SETTINGS_TOOLBOX, false);
    setToolBoxVisibility(LAYERS_TOOLBOX, false);
    setToolBoxVisibility(PIPELINE_CHOOSER_TOOLBOX, !d->isCurrentlyRunningPipeline);
    setToolBoxVisibility(PIPELINE_CONTROL_TOOLBOX, d->isCurrentlyRunningPipeline);
}

void PipelineWorkspace::runPipeline(dtkSmartPointer<PipelineInstance> pipeline)
{
    pipeline->setup();
    getToolBox<PipelineControlToolBox>()->setPipeline(pipeline);
    d->isCurrentlyRunningPipeline = true;
    clear();
}

void PipelineWorkspace::stopPipeline()
{
    d->isCurrentlyRunningPipeline = false;
    clear();
}

void PipelineWorkspace::setToolBoxVisibility(QString identifier, bool visible)
{
    setToolBoxVisibility(getToolBox(identifier), visible);
}

void PipelineWorkspace::setToolBoxVisibility(medToolBox* toolBox, bool visible)
{
    toolBox->show();
    toolBox->header()->setVisible(visible);
    toolBox->body()->setVisible(visible);
}

medToolBox* PipelineWorkspace::addToolBox(QString identifier)
{
    return insertToolBox(toolBoxes().count(), identifier);
}

medToolBox* PipelineWorkspace::insertToolBox(int index, QString identifier)
{
    medToolBox* toolBox = medToolBoxFactory::instance()->createToolBox(identifier);

    if (toolBox)
    {
        insertToolBox(index, toolBox);
        return toolBox;
    }
    else
    {
        THROW_METHOD_EXCEPTION("Cannot create toolbox " + identifier);
    }
}

medToolBox* PipelineWorkspace::getToolBox(QString identifier)
{
    if (identifier == MOUSE_INTERACTION_TOOLBOX)
    {
        return getMouseInteractionToolBox();
    }
    else if (identifier == VIEW_SETTINGS_TOOLBOX)
    {
        return getNavigatorToolBox();
    }
    else if (identifier == LAYERS_TOOLBOX)
    {
        return getLayersToolBox();
    }
    else if (identifier == PROGRESSION_STACK_TOOLBOX)
    {
        return getProgressionStackToolBox();
    }
    else
    {
        foreach (medToolBox* toolBox, toolBoxes())
        {
            if (toolBox->identifier() == identifier)
            {
                return toolBox;
            }
        }
    }

    THROW_METHOD_EXCEPTION("Cannot find toolbox " + identifier);
}

void PipelineWorkspace::removeToolBox(QString identifier)
{
    removeToolBox(getToolBox(identifier));
}

dtkSmartPointer<medAbstractData> PipelineWorkspace::getToolBoxOutput(QString identifier)
{
    return getToolBoxOutput(getToolBox(identifier));
}

dtkSmartPointer<medAbstractData> PipelineWorkspace::getToolBoxOutput(medToolBox* toolBox)
{
    medAbstractSelectableToolBox* abstractToolBox = qobject_cast<medAbstractSelectableToolBox*>(toolBox);

    if (abstractToolBox)
    {
        return abstractToolBox->processOutput();
    }
    else
    {
        THROW_METHOD_EXCEPTION(toolBox->metaObject()->className() + " does not derive from "
                        + medAbstractSelectableToolBox::staticMetaObject.className());
    }
}

bool PipelineWorkspace::hasDroppedResults()
{
    return !getDroppedResults().isEmpty();
}

dtkSmartPointer<medAbstractData> PipelineWorkspace::getDroppedResult()
{
    QList<dtkSmartPointer<medAbstractData> > droppedResults = getDroppedResults();

    if (droppedResults.isEmpty())
    {
        return nullptr;
    }
    else
    {
        return droppedResults.first();
    }
}

dtkSmartPointer<medAbstractData> PipelineWorkspace::getDroppedResultOrToolBoxOutput(QString identifier)
{
    dtkSmartPointer<medAbstractData> result = getDroppedResult();

    if (result.data() != nullptr)
    {
        return result;
    }
    else
    {
        return getToolBoxOutput(identifier);
    }
}

QList<dtkSmartPointer<medAbstractData> > PipelineWorkspace::getDroppedResults()
{
    return getToolBox<PipelineControlToolBox>()->getDroppedResults();
}

medAbstractLayeredView* PipelineWorkspace::getView(int containerIndex, int tabIndex)
{
    return qobject_cast<medAbstractLayeredView*>(getContainer(containerIndex, tabIndex)->view());
}

medViewContainer* PipelineWorkspace::getContainer(int containerIndex, int tabIndex)
{
    return tabbedViewContainers()->containersInTab(tabIndex).at(containerIndex);
}

dtkSmartPointer<medAbstractData> PipelineWorkspace::getLayerData(int layerIndex, int containerIndex, int tabIndex)
{
    return qobject_cast<medAbstractLayeredView*>(getContainer(containerIndex, tabIndex)->view())->layerData(layerIndex);
}

medViewContainer* PipelineWorkspace::addData(dtkSmartPointer<medAbstractData> data, int containerIndex, int tabIndex)
{
    medViewContainer* container = nullptr;

    if (tabbedViewContainers()->count() <= tabIndex)
    {
        for (int i = (tabIndex - tabbedViewContainers()->count()); i >= 0; i--)
        {
            container = tabbedViewContainers()->addContainerInTabNamed(QString("Data %0").arg(containerIndex));
            container->setMultiLayered(true);
            container->setClosingMode(medViewContainer::CLOSE_VIEW);
        }
    }

    if (tabbedViewContainers()->containersInTab(tabIndex).length() <= containerIndex)
    {
        for (int i = (containerIndex - tabbedViewContainers()->containersInTab(tabIndex).length()); i >= 0; i--)
        {
            container = container->splitHorizontally();
            container->setMultiLayered(true);
            container->setClosingMode(medViewContainer::CLOSE_VIEW);
        }
    }

    if (!container)
    {
        container = getContainer(containerIndex, tabIndex);
    }

    container->addData(data);

    foreach (medAttachedData* attachedData, data->attachedData())
    {
        PipelineAttachedData* pipelineAttachedData = dynamic_cast<PipelineAttachedData*>(attachedData);
        if (pipelineAttachedData)
        {
            int layer = static_cast<medAbstractLayeredView*>(container->view())->layersCount() - 1;
            if (pipelineAttachedData->hasWindowLevel())
            {
                setWindowLevel(pipelineAttachedData->getWindow(), pipelineAttachedData->getLevel(), layer, containerIndex, tabIndex);
            }
            if (pipelineAttachedData->hasRenderingParameters())
            {
                setRenderingParameters(pipelineAttachedData->getRenderingParameters(), layer, containerIndex, tabIndex);
            }
            break;
        }
    }

    return container;
}

void PipelineWorkspace::setWindowLevel(double window, double level, int layerIndex, int containerIndex, int tabIndex)
{
    medAbstractLayeredView* layeredView = qobject_cast<medAbstractLayeredView*>(getContainer(containerIndex, tabIndex)->view());
    foreach (medAbstractInteractor* interactor, layeredView->layerInteractors(layerIndex))
    {
        medAbstractImageViewInteractor* viewInteractor = qobject_cast<medAbstractImageViewInteractor*>(interactor);
        if (viewInteractor)
        {
            QHash<QString, QVariant> windowLevel;
            windowLevel["Window"] = QVariant(window);
            windowLevel["Level"] = QVariant(level);
            viewInteractor->setWindowLevel(windowLevel);
        }
    }
}

void PipelineWorkspace::setRenderingParameters(RenderingParameters renderingParameters, int layerIndex, int containerIndex, int tabIndex)
{
    medAbstractLayeredView* layeredView = qobject_cast<medAbstractLayeredView*>(getContainer(containerIndex, tabIndex)->view());
    foreach (medAbstractInteractor* interactor, layeredView->layerInteractors(layerIndex))
    {
        QString identifier(interactor->identifier());
        if (identifier.contains("vtkDataMeshInteractor") ||
            identifier.contains("EPMap"))
        {
            foreach (medAbstractParameterL* parameter, interactor->linkableParameters())
            {
                if (parameter->name() == "Color")
                {
                    qobject_cast<medStringListParameterL*>(parameter)->setValue(renderingParameters.color);
                }
                else if (parameter->name() == "Opacity")
                {
                    qobject_cast<medDoubleParameterL*>(parameter)->setValue(renderingParameters.opacity);
                }
                else if (parameter->name() == "Rendering")
                {
                    qobject_cast<medStringListParameterL*>(parameter)->setValue(renderingParameters.renderingMode);
                }

                //Optional features, only for mapped meshes
                else if ((parameter->name() == "Attributes") && (renderingParameters.attribute != ""))
                {
                    qobject_cast<medStringListParameterL*>(parameter)->setValue(renderingParameters.attribute);
                }
                //Here assumption is: if we set Min, then we set Max. If min = max (as is with the default values), we do nothing.
                else if ((parameter->name() == "Min") && (renderingParameters.minRange != renderingParameters.maxRange))
                {
                    qobject_cast<medDoubleParameterL*>(parameter)->setValue(renderingParameters.minRange);
                }
                else if ((parameter->name() == "Max") && (renderingParameters.maxRange != renderingParameters.minRange))
                {
                    qobject_cast<medDoubleParameterL*>(parameter)->setValue(renderingParameters.maxRange);
                }
            }
        }
    }
}

void PipelineWorkspace::setColorRange(double minimumValue, double maximumValue, int containerIndex, int tabIndex)
{
    medAbstractView* view = this->getContainer(containerIndex, tabIndex)->view();
    vtkImageView2D* view2d = static_cast<medVtkViewBackend*>(view->backend())->view2D;

    double range[2] = {minimumValue, maximumValue};
    view2d->SetColorRange(range);
    view->viewWidget()->update();
}

void PipelineWorkspace::setLookUpTable(QString lookupTable, int containerIndex, int tabIndex)
{
    medAbstractView* view = this->getContainer(containerIndex, tabIndex)->view();
    vtkImageView2D* view2d = static_cast<medVtkViewBackend*>(view->backend())->view2D;

    view2d->SetLookupTable(vtkLookupTableManager::GetLookupTable(lookupTable.toStdString()));
    view->viewWidget()->update();
}

void PipelineWorkspace::secureImport(dtkSmartPointer<medAbstractData> data)
{
    QMutexLocker(&d->importMutex);

    d->currentImport = data;
    d->currentImportIndex = data->dataIndex();

    if (medDataManager::instance()->retrieveData(d->currentImportIndex) != nullptr)
    {
        medDataManager::instance()->removeData(d->currentImportIndex);
    }
    else
    {
        importCurrentData(d->currentImportIndex);
    }

    while (d->currentImport.data() != nullptr)
    {
        qApp->processEvents();
    }
}

void PipelineWorkspace::setVolumeRendering(int containerIndex, int tabIndex)
{
    medAbstractImageView* imageView = qobject_cast<medAbstractImageView*>(getContainer(containerIndex, tabIndex)->view());
    imageView->setOrientation(medImageView::VIEW_ORIENTATION_3D);

    foreach (medAbstractNavigator* navigator, imageView->navigators())
    {
        foreach (medAbstractParameterL* extraParameter, navigator->linkableParameters())
        {
            if (extraParameter->name() == "3D Mode")
            {
                qobject_cast<medStringListParameterL*>(extraParameter)->setValue("VR");
                return;
            }
        }
    }
}

void PipelineWorkspace::setCoronalRendering(int containerIndex, int tabIndex)
{
    medAbstractImageView* imageView = qobject_cast<medAbstractImageView*>(getContainer(containerIndex, tabIndex)->view());
    imageView->setOrientation(medImageView::VIEW_ORIENTATION_CORONAL);
}

void PipelineWorkspace::setSagittalRendering(int containerIndex, int tabIndex)
{
    medAbstractImageView* imageView = qobject_cast<medAbstractImageView*>(getContainer(containerIndex, tabIndex)->view());
    imageView->setOrientation(medImageView::VIEW_ORIENTATION_SAGITTAL);
}

void PipelineWorkspace::displayException(QString title, std::exception& e)
{
    emit exceptionAnnounced(title, e.what());
}

void PipelineWorkspace::open(const medDataIndex& index)
{
    if (d->isCurrentlyRunningPipeline)
    {
        medAbstractWorkspaceLegacy::open(index);
    }
    else
    {
        getToolBox<PipelineChooserToolBox>()->continueExistingPipeline(index);
    }
}

void PipelineWorkspace::importCurrentData(const medDataIndex& index)
{
    QMutexLocker(&d->importMutex);

    if (d->currentImportIndex == index)
    {
        d->currentImportId = medDataManager::instance()->importData(d->currentImport, true);
    }
}

void PipelineWorkspace::updateImportedDataIndex(const medDataIndex& index, QUuid importId)
{
    QMutexLocker(&d->importMutex);

    if (d->currentImportId == importId)
    {
        d->currentImport->setDataIndex(index);
        d->currentImport = nullptr;
        d->currentImportIndex = medDataIndex();
        d->currentImportId = 0;
    }
}

void PipelineWorkspace::displayExceptionBox(QString title, QString message)
{
    qDebug() << title;
    qDebug() << message;

    medMessageController::instance()->showError(title, 3000);

    QMessageBox messsageBox;
    QFontMetrics fontMetrics(messsageBox.font());
    int width = fontMetrics.boundingRect(message).width();
    QSpacerItem* horizontalSpacer = new QSpacerItem(width, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout* layout = static_cast<QGridLayout*>(messsageBox.layout());
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
    messsageBox.setText(title);
    messsageBox.setInformativeText(message);
    messsageBox.setStandardButtons(QMessageBox::Ok);
    messsageBox.exec();
}

} // namespace med::pipeline
