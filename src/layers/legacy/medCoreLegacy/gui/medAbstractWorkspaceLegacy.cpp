/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medAbstractWorkspaceLegacy.h"

#include <medAbstractLayeredView.h>
#include <medAbstractView.h>
#include <medBoolParameterL.h>
#include <medDataManager.h>
#include <medLayerParameterGroupL.h>
#include <medLinkMenu.h>
#include <medListWidget.h>
#include <medMetaDataKeys.h>
#include <medParameterGroupManagerL.h>
#include <medParameterPoolL.h>
#include <medPoolIndicatorL.h>
#include <medTabbedViewContainers.h>
#include <medToolBox.h>
#include <medToolBoxBody.h>
#include <medToolBoxHeader.h>
#include <medViewContainer.h>
#include <medViewContainerManager.h>
#include <medViewParameterGroupL.h>

class medAbstractWorkspaceLegacyPrivate
{
public:
    QWidget *parent;

    QString name;

    bool databaseVisibility;
    bool toolBoxesVisibility;
    bool userLayerPoolable;
    bool userLayerClosable;
    bool userViewPoolable;

    medTabbedViewContainers * viewContainerStack;
    QHash <QListWidgetItem*, QUuid> containerForLayerWidgetsItem;

    QList<medToolBox*> toolBoxes;
    medToolBox *selectionToolBox;
    medToolBox *layersToolBox;
    medToolBox *layerListToolBox;
    medToolBox *interactorToolBox;
    medToolBox *navigatorToolBox;
    medToolBox *mouseInteractionToolBox;
    medListWidget* layerListWidget;

    QList<QListWidgetItem*> selectedLayers;
    QHash<int,medPoolIndicatorL*> poolIndicators;

    medParameterPoolL *temporaryPoolForInteractors;

    medLinkMenu *viewLinkMenu;
    medLinkMenu *layerLinkMenu;

    medProgressionStack* progressionStack;
};

medAbstractWorkspaceLegacy::medAbstractWorkspaceLegacy(QWidget *parent)
    : QObject(parent), d(new medAbstractWorkspaceLegacyPrivate)
{
    d->parent = parent;

    d->selectionToolBox = new medToolBox;
    d->selectionToolBox->setTitle("Selection");
    d->selectionToolBox->header()->hide();
    d->selectionToolBox->hide();

    d->viewContainerStack = new medTabbedViewContainers(this, parent);

    connect(d->viewContainerStack, SIGNAL(containersSelectedChanged()), this, SLOT(updateNavigatorsToolBox()), Qt::UniqueConnection);

    d->databaseVisibility = true;
    d->toolBoxesVisibility = true;

    d->mouseInteractionToolBox = new medToolBox;
    d->mouseInteractionToolBox->setTitle("Mouse Interaction");
    d->mouseInteractionToolBox->hide();
    d->selectionToolBox->addWidget(d->mouseInteractionToolBox);

    d->navigatorToolBox = new medToolBox;
    d->navigatorToolBox->setTitle("View settings");
    d->navigatorToolBox->hide();
    d->selectionToolBox->addWidget(d->navigatorToolBox);

    d->layersToolBox = new medToolBox;
    d->layersToolBox->setTitle("Layer settings");
    d->layersToolBox->show();

    d->layerListToolBox = new medToolBox;
    d->layerListToolBox->header()->hide();
    d->layersToolBox->addWidget(d->layerListToolBox);

    d->interactorToolBox = new medToolBox;
    d->interactorToolBox->header()->hide();
    d->layersToolBox->addWidget(d->interactorToolBox);

    d->selectionToolBox->addWidget(d->layersToolBox);

    d->progressionStack = new medProgressionStack();
    d->selectionToolBox->addWidget(d->progressionStack);

    d->layerListToolBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    d->layerListWidget = nullptr;

    d->temporaryPoolForInteractors = new medParameterPoolL(this);

    this->setUserLayerClosable(true);
    this->setUserLayerPoolable(true);
    this->setUserViewPoolable(true);

    d->viewLinkMenu = nullptr;
    d->layerLinkMenu = nullptr;
}

medAbstractWorkspaceLegacy::~medAbstractWorkspaceLegacy(void)
{
    delete d;
    d = nullptr;
}

void medAbstractWorkspaceLegacy::addToolBox(medToolBox *toolbox)
{
    toolbox->setWorkspace(this);
    d->toolBoxes.append(toolbox);
    d->selectionToolBox->addWidget(toolbox);
}

void medAbstractWorkspaceLegacy::removeToolBox(medToolBox *toolbox)
{
    toolbox->setWorkspace(nullptr);
    d->toolBoxes.removeOne(toolbox);
    d->selectionToolBox->removeWidget(toolbox);
}

QList<medToolBox*> medAbstractWorkspaceLegacy::toolBoxes() const
{
    return d->toolBoxes;
}

medToolBox* medAbstractWorkspaceLegacy::selectionToolBox() const
{
    return d->selectionToolBox;
}

void medAbstractWorkspaceLegacy::setDatabaseVisibility(bool visibility)
{
    d->databaseVisibility = visibility;
}

bool medAbstractWorkspaceLegacy::isDatabaseVisible(void) const
{
    return d->databaseVisibility;
}

medTabbedViewContainers* medAbstractWorkspaceLegacy::tabbedViewContainers() const
{
    return d->viewContainerStack;
}

void medAbstractWorkspaceLegacy::clear()
{
    this->setupTabbedViewContainer();
    this->clearWorkspaceToolBoxes();
    return;
}

void medAbstractWorkspaceLegacy::setupTabbedViewContainer()
{
    if (!tabbedViewContainers()->count())
    {
        this->tabbedViewContainers()->addContainerInTabNamed(this->name());
    }
    this->tabbedViewContainers()->lockTabs();
    this->tabbedViewContainers()->setKeepLeastOne(true);
}

void medAbstractWorkspaceLegacy::setToolBoxesVisibility (bool value)
{
    d->toolBoxesVisibility = value;
}

bool medAbstractWorkspaceLegacy::areToolBoxesVisible() const
{
    return d->toolBoxesVisibility;
}

void medAbstractWorkspaceLegacy::clearWorkspaceToolBoxes()
{
    for(medToolBox* tb : d->toolBoxes)
    {
        tb->body()->clear();
    }
}

void medAbstractWorkspaceLegacy::addNewTab()
{
    QString tabName = QString("%1 %2").arg(this->name()).arg(d->viewContainerStack->count());
    d->viewContainerStack->addContainerInTabNamed(tabName);
}

void medAbstractWorkspaceLegacy::updateNavigatorsToolBox()
{
    d->navigatorToolBox->body()->clear();

    medAbstractView* view = nullptr;
    QList<QWidget*>  navigators;
    QStringList viewType;

    for(QUuid uuid : d->viewContainerStack->containersSelected())
    {
        medViewContainer *container = medViewContainerManager::instance()->container(uuid);
        // update the toolbox when the content of the view change
        view = container->view();

        // add nothing if the view is empty
        if(!view)
            continue;
        // get only one navigator per view type
        if(!viewType.contains(view->identifier()))
        {
            viewType << view->identifier();
            navigators << view->navigatorWidget();
        }
    }
    // add the navigators widgets
    d->navigatorToolBox->show();
    for(QWidget* navigator : navigators)
    {
        d->navigatorToolBox->addWidget(navigator);
        navigator->show();
    }

    // add link parameter
    if(view && d->userViewPoolable)
    {
        QWidget *linkWidget = buildViewLinkMenu();
        d->navigatorToolBox->addWidget(linkWidget);
    }

    // update the mouse interaction and layer toolboxes according to the selected containers
    this->updateMouseInteractionToolBox();
    this->updateLayersToolBox();
}

void medAbstractWorkspaceLegacy::updateMouseInteractionToolBox()
{
    d->mouseInteractionToolBox->body()->clear();

    QList<QWidget*>  navigators;
    QStringList viewType;
    for(QUuid uuid : d->viewContainerStack->containersSelected())
    {
        medViewContainer *container = medViewContainerManager::instance()->container(uuid);
        // update the toolbox when the content of the view change
        medAbstractView* view = container->view();
        // add nothing if the view is empty
        if(!view)
            continue;
        // get only one navigator per view type
        if(!viewType.contains(view->identifier()))
        {
            viewType << view->identifier();
            navigators << view->mouseInteractionWidget();
        }
    }
    // add the navigators widgets
    d->mouseInteractionToolBox->show();
    for(QWidget* navigator : navigators)
    {
        d->mouseInteractionToolBox->addWidget(navigator);
        navigator->show();
    }
}

QList<int> medAbstractWorkspaceLegacy::getSelectedLayerIndices()
{
    QList<int> layerIndices;
    for(QListWidgetItem *item : d->selectedLayers)
    {
        layerIndices.append(item->data(Qt::UserRole).toInt());
    }
    return layerIndices;
}

void medAbstractWorkspaceLegacy::handleLayerSelectionChange()
{
    this->updateInteractorsToolBox();
    emit layerSelectionChanged(getSelectedLayerIndices());
}

void medAbstractWorkspaceLegacy::resetCameraOnSelectedLayer(QListWidgetItem *item)
{
    QUuid uuid = d->containerForLayerWidgetsItem.value(item);
    medViewContainer *container = medViewContainerManager::instance()->container(uuid);
    if (container)
    {
        medAbstractLayeredView *layeredView = dynamic_cast<medAbstractLayeredView*>(container->view());
        if (layeredView)
        {
            int currentLayer = item->data(Qt::UserRole).toInt();
            layeredView->resetCameraOnLayer(currentLayer);
        }
    }
}

void medAbstractWorkspaceLegacy::updateLayersToolBox()
{
    d->layerListToolBox->body()->clear();
    d->containerForLayerWidgetsItem.clear();
    d->selectedLayers.clear();
    d->poolIndicators.clear();

    delete d->layerListWidget;
    d->layerListWidget = new medListWidget;
    d->layerListWidget->setAlternatingRowColors(true);
    d->layerListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    d->layerListWidget->setFocusPolicy(Qt::NoFocus);
    d->layerListWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    connect(d->layerListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(changeCurrentLayer(int)));
    connect(d->layerListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(handleLayerSelectionChange()));
    connect(d->layerListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(resetCameraOnSelectedLayer(QListWidgetItem*)));

    for(QUuid uuid : d->viewContainerStack->containersSelected())
    {
        // fill the layer widget
        medViewContainer *container = medViewContainerManager::instance()->container(uuid);
        medAbstractLayeredView* layeredView = dynamic_cast<medAbstractLayeredView*>(container->view());
        if(layeredView)
        {
            for(int layer = layeredView->layersCount() - 1; layer >= 0; --layer)
            {
                if(layer < 0)
                    break;

                medAbstractData *data = layeredView->layerData(layer);
                if(!data)
                    continue;

                QWidget *layerWidget = new QWidget;
                layerWidget->setObjectName("layerWidget");

                QString name = medMetaDataKeys::SeriesDescription.getFirstValue(data,"<i>no name</i>");

                QHBoxLayout* layout = new QHBoxLayout(layerWidget);
                layout->setContentsMargins(0,0,5,0);

                medBoolParameterL* visibilityParam = dynamic_cast<medBoolParameterL*>(layeredView->visibilityParameter(layer));
                QPushButton* thumbnailButton = visibilityParam->getPushButton();

                QFont myFont;
                QFontMetrics fm(myFont);
                QString text = fm.elidedText(name, Qt::ElideRight, 160);
                QLabel *layerName = new QLabel("<b>"+text+"</b>", layerWidget);
                layerName->setToolTip(name);

                layout->addWidget(thumbnailButton);
                layout->addWidget(layerName);
                layout->addStretch();

                for(medAbstractInteractor *interactor : layeredView->layerInteractors(layer))
                {
                    if(interactor->layerWidget())
                        layout->addWidget(interactor->layerWidget());
                }

                if(d->userLayerPoolable)
                {
                    medPoolIndicatorL *poolIndicator = new medPoolIndicatorL;
                    layout->addWidget(poolIndicator);
                    d->poolIndicators.insert(d->layerListWidget->count(), poolIndicator);

                    QList<medLayerParameterGroupL*> layerGroups = medParameterGroupManagerL::instance()->layerGroups(layeredView, data);
                    for(medLayerParameterGroupL *layerGroup : layerGroups)
                    {
                        poolIndicator->addColorIndicator(layerGroup->color(), layerGroup->name());
                        connect(layerGroup, SIGNAL(groupColorChanged(QColor,QColor)), poolIndicator, SLOT(replaceColorIndicator(QColor,QColor)));
                    }
                }

                if(d->userLayerClosable)
                {
                    QPushButton *removeButton = new QPushButton;
                    removeButton->setProperty("row", d->layerListWidget->count());
                    removeButton->setIcon(QIcon(":/icons/cross.svg"));
                    removeButton->setIconSize(QSize(15,15));
                    removeButton->setFixedSize(20,20);

                    layout->addWidget(removeButton);
                    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeLayer()));

                }

                layerWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
                layerWidget->resize(d->selectionToolBox->sizeHint().width(), 25);

                QListWidgetItem * item = new QListWidgetItem;
                item->setData(Qt::UserRole, layer);
                d->containerForLayerWidgetsItem.insert(item, uuid);
                item->setSizeHint(QSize(layerWidget->sizeHint().width(), 25));
                d->layerListWidget->addItem(item);
                d->layerListWidget->setItemWidget(item, layerWidget);
                layerWidget->setFocusPolicy(Qt::NoFocus);

                d->layerListWidget->blockSignals(true);
                if(layeredView->currentLayer() == item->data(Qt::UserRole).toUInt())
                {
                    d->layerListWidget->setCurrentItem(item);
                    item->setSelected(true);
                }
                d->layerListWidget->blockSignals(false);

            }
            // add the layer widgets
            if (layeredView->layersCount() > 0)
            {
                d->layersToolBox->show();
                d->layerListToolBox->addWidget(d->layerListWidget);

                // resize layerListWidget to number of item
                d->layerListWidget->setFixedHeight(d->layerListWidget->sizeHintForRow(0) * d->layerListWidget->count() + 2 * d->layerListWidget->frameWidth());
                d->layerListWidget->show();
            }
        }
    }

    this->updateInteractorsToolBox();
}

void medAbstractWorkspaceLegacy::changeCurrentLayer(int row)
{
    QListWidgetItem* item = d->layerListWidget->item(row);
    QUuid uuid = d->containerForLayerWidgetsItem.value(item);
    medViewContainer* container = medViewContainerManager::instance()->container(uuid);
    if(!container)
        return;

    medAbstractLayeredView *layeredView = dynamic_cast<medAbstractLayeredView*>(container->view());
    if(!layeredView)
        return;

    int currentLayer = item->data(Qt::UserRole).toInt();
    layeredView->setCurrentLayer(currentLayer);

    d->selectedLayers = d->layerListWidget->selectedItems();

    this->updateInteractorsToolBox();

    // update the mouse interaction  according to the selected layers
    this->updateMouseInteractionToolBox();
}

void medAbstractWorkspaceLegacy::updateInteractorsToolBox()
{
    medViewContainerManager *containerMng =  medViewContainerManager::instance();
    for(QUuid uuid : d->viewContainerStack->containersSelected())
    {
        containerMng->container(uuid)->highlight();
    }
    d->interactorToolBox->hide();
    d->interactorToolBox->body()->clear();

    if(!d->layerListWidget)
        return;

    if(d->layerListWidget->selectedItems().count() == 0)
    {
        for(QListWidgetItem* item : d->selectedLayers)
        {
            d->layerListWidget->blockSignals(true);
            item->setSelected(true);
            d->layerListWidget->blockSignals(false);
        }
    }
    else d->selectedLayers = d->layerListWidget->selectedItems();

    QListWidgetItem* item = d->layerListWidget->currentItem();
    if(!item)
        return;

    int currentLayer = item->data(Qt::UserRole).toInt();
    QList<QString> interactorsIdentifier;
    QUuid containerUuid = d->containerForLayerWidgetsItem.value(item);
    medViewContainer *container = containerMng->container(containerUuid);
    container->highlight("#FF8844");

    medAbstractLayeredView *view = dynamic_cast<medAbstractLayeredView*>(container->view());

    // add link parameter
    if(view &&  d->userLayerPoolable)
    {
        QWidget *linkMenu = buildLayerLinkMenu(d->layerListWidget->selectedItems());
        d->interactorToolBox->addWidget(linkMenu);

        d->interactorToolBox->show();
    }

    // If we have a multiSelection we just want to colorise the container that have at least one layer selected.
    if(d->layerListWidget->selectedItems().size() > 1)
    {
        for(QListWidgetItem* item : d->layerListWidget->selectedItems())
        {
            QUuid containerUuid = d->containerForLayerWidgetsItem.value(item);
            medViewContainer *container = containerMng->container(containerUuid);
            container->highlight("#FF8844");
        }

        buildTemporaryPool();

        return;
    }

    for(medAbstractInteractor* interactor : view->layerInteractors(currentLayer))
    {
        QString interactorIdentifier = interactor->identifier();
        if(!interactorsIdentifier.contains(interactorIdentifier))
        {
            interactorsIdentifier << interactorIdentifier;
            QGroupBox *groupBox = new QGroupBox;
            QVBoxLayout *groupBoxLayout = new QVBoxLayout(groupBox);

            groupBoxLayout->setContentsMargins(0,0,0,0);
            QWidget *intercatorWidget = interactor->toolBoxWidget();
            if(intercatorWidget)
            {
                groupBoxLayout->addWidget(interactor->toolBoxWidget());
                d->interactorToolBox->addWidget(groupBox);
                interactor->toolBoxWidget()->show();
            }
        }
    }
    d->interactorToolBox->show();

    buildTemporaryPool();
}


void medAbstractWorkspaceLegacy::removeLayer()
{
    QPushButton *button = dynamic_cast<QPushButton*>(this->sender());
    if(!button)
        return;

    int row = button->property("row").toInt();
    QListWidgetItem* item = d->layerListWidget->item(row);
    QUuid containerUuid = d->containerForLayerWidgetsItem.value(item);

    int layer = item->data(Qt::UserRole).toInt();

    medAbstractLayeredView *layerView = dynamic_cast<medAbstractLayeredView *>(medViewContainerManager::instance()->container(containerUuid)->view());
    if(!layerView)
        return;

    layerView->removeLayer(layer);
    if (layerView->layersCount() == 0)
    {
        if (medViewContainerManager::instance()->container(containerUuid)->closingMode()
                == medViewContainer::CLOSE_CONTAINER)
        {
            medViewContainerManager::instance()->container(containerUuid)->checkIfStillDeserveToLiveContainer();
        }
        else
        {
            // For containers that we want to keep even if there are no views/data in it, as in Filtering
            medViewContainerManager::instance()->container(containerUuid)->removeView();
        }
    }

    this->updateLayersToolBox();
}


void medAbstractWorkspaceLegacy::buildTemporaryPool()
{
    medViewContainerManager *containerMng =  medViewContainerManager::instance();
    d->temporaryPoolForInteractors->clear();

    for(QListWidgetItem* item : d->layerListWidget->selectedItems())
    {
        QUuid containerUuid = d->containerForLayerWidgetsItem.value(item);
        medViewContainer *container = containerMng->container(containerUuid);
        int layer = item->data(Qt::UserRole).toInt();

        medAbstractLayeredView *view = dynamic_cast<medAbstractLayeredView*>(container->view());

        for(medAbstractInteractor* interactor : view->layerInteractors(layer))
        {
            d->temporaryPoolForInteractors->append(interactor->linkableParameters());
        }
    }
}

void medAbstractWorkspaceLegacy::open(const medDataIndex &index)
{
    // Called after right click->View on a series/study, or double-click on a series
    QList<QUuid> containersSelected = d->viewContainerStack->containersSelected();
    if(containersSelected.size() != 1)
        return;

    medViewContainer *container = medViewContainerManager::instance()->container(containersSelected.first());
    if(index.isValidForSeries())
    {
        container->addData(medDataManager::instance()->retrieveData(index));
    }
    else if(index.isValidForStudy())
    {
        // We get the list of each series from that study index, and open it
        QList<medDataIndex> seriesList = medDataManager::instance()->getSeriesListFromStudy(index);
        if (seriesList.count() > 0)
        {
            for(medDataIndex seriesIndex : seriesList)
            {
                container->addData(medDataManager::instance()->retrieveData(seriesIndex));
            }
        }
    }
}

void medAbstractWorkspaceLegacy::setUserLayerPoolable(bool poolable)
{
    d->userLayerPoolable = poolable;
    this->updateLayersToolBox();
}

void medAbstractWorkspaceLegacy::setUserViewPoolable(bool poolable)
{
    d->userViewPoolable = poolable;
    this->updateNavigatorsToolBox();
}

void medAbstractWorkspaceLegacy::setUserLayerClosable(bool Closable)
{
    d->userLayerClosable = Closable;
    this->updateLayersToolBox();
}

bool medAbstractWorkspaceLegacy::isUserLayerPoolable() const
{
    return d->userLayerPoolable;
}

bool medAbstractWorkspaceLegacy::isUserViewPoolable() const
{
    return d->userViewPoolable;
}

bool medAbstractWorkspaceLegacy::isUserLayerClosable() const
{
    return d->userLayerClosable;
}


QWidget* medAbstractWorkspaceLegacy::buildViewLinkMenu()
{
    QWidget *linkWidget = new QWidget;
    QHBoxLayout* linkLayout = new QHBoxLayout(linkWidget);
    linkLayout->setContentsMargins(80, 5, 5, 5);
    linkLayout->setSpacing(0);

    d->viewLinkMenu = new medLinkMenu(linkWidget);
    connect(d->viewLinkMenu, SIGNAL(groupChecked(QString)), this, SLOT(addViewstoGroup(QString)));
    connect(d->viewLinkMenu, SIGNAL(groupUnchecked(QString)), this, SLOT(removeViewsFromGroup(QString)));
    connect(d->viewLinkMenu, SIGNAL(groupCreated(QString)), this, SLOT(addViewGroup(QString)));
    connect(d->viewLinkMenu, SIGNAL(groupDeleted(QString)), this, SLOT(removeViewGroup(QString)));
    connect(d->viewLinkMenu, SIGNAL(groupColorChangeRequest(QString,QColor)), this, SLOT(changeViewGroupColor(QString, QColor)));

    medAbstractView* view = nullptr;
    QStringList viewType;
    QList<medAbstractView*> views;
    QList<medAbstractParameterL*>  viewsParams;

    for(QUuid uuid : d->viewContainerStack->containersSelected())
    {
        medViewContainer *container = medViewContainerManager::instance()->container(uuid);
        view = container->view();

        // add nothing if the view is empty
        if(!view)
            continue;

        views << view;

        // get only one navigator per view type
        if(!viewType.contains(view->identifier()))
        {
            viewType << view->identifier();
            viewsParams.append(view->linkableParameters());
        }
    }

    QStringList paramNames;

    // build a stringList witth all the param names
    for(medAbstractParameterL* viewParam : viewsParams)
    {
        paramNames << viewParam->name();
    }

    QList<medAbstractParameterGroupL*>  abstractGroupsList;
    QStringList selectedGroups;
    QStringList partiallySelectedGroups;

    auto viewGroupsList = medParameterGroupManagerL::instance()->viewGroups(this->identifier());
    for(medViewParameterGroupL *viewGroup : viewGroupsList)
    {
        abstractGroupsList.append(viewGroup);
        bool selected = true;
        bool partiallySelected = false;

        for(medAbstractView *view : views)
        {
            if(!viewGroup->impactedViews().contains(view))
                selected = false;
            else
                partiallySelected = true;
        }
        if(selected)
            selectedGroups << viewGroup->name();
        else if(partiallySelected)
            partiallySelectedGroups << viewGroup->name();
    }

    d->viewLinkMenu->setAvailableParameters( paramNames );
    d->viewLinkMenu->setGroups(abstractGroupsList);
    d->viewLinkMenu->setSelectedGroups(selectedGroups);
    d->viewLinkMenu->setPartiallySelectedGroups(partiallySelectedGroups);

    linkLayout->addWidget(new QLabel(tr("Link view properties: ")));
    linkLayout->addWidget(d->viewLinkMenu);

    return linkWidget;
}


QWidget* medAbstractWorkspaceLegacy::buildLayerLinkMenu(QList<QListWidgetItem*> selectedlayers)
{
    QStringList paramNames;
    QList<medAbstractParameterL*> layersParams;
    QMultiHash<medAbstractLayeredView*, unsigned int> layers;
    medViewContainerManager *containerMng =  medViewContainerManager::instance();

    for(QListWidgetItem *item : selectedlayers)
    {
        int currentLayer = item->data(Qt::UserRole).toInt();
        QList<QString> interactorsIdentifier;

        QUuid containerUuid = d->containerForLayerWidgetsItem.value(item);
        medViewContainer *container = containerMng->container(containerUuid);
        medAbstractLayeredView *view = dynamic_cast<medAbstractLayeredView*>(container->view());

        for(medAbstractInteractor* interactor : view->layerInteractors(currentLayer))
        {
            QString interactorIdentifier = interactor->identifier();
            if(!interactorsIdentifier.contains(interactorIdentifier))
            {
                interactorsIdentifier << interactorIdentifier;
                layersParams << interactor->linkableParameters();
            }
        }

        layers.insert(view, currentLayer);
    }

    QWidget *linkWidget = new QWidget;
    QHBoxLayout* linkLayout = new QHBoxLayout(linkWidget);
    linkLayout->setContentsMargins(80, 5, 5, 5);
    linkLayout->setSpacing(0);

    d->layerLinkMenu = new medLinkMenu(linkWidget);
    connect(d->layerLinkMenu, SIGNAL(groupChecked(QString)), this, SLOT(addLayerstoGroup(QString)));
    connect(d->layerLinkMenu, SIGNAL(groupUnchecked(QString)), this, SLOT(removeLayersFromGroup(QString)));

    connect(d->layerLinkMenu, SIGNAL(groupCreated(QString)), this, SLOT(addLayerGroup(QString)));
    connect(d->layerLinkMenu, SIGNAL(groupDeleted(QString)), this, SLOT(removeLayerGroup(QString)));
    connect(d->layerLinkMenu, SIGNAL(groupColorChangeRequest(QString,QColor)), this, SLOT(changeLayerGroupColor(QString, QColor)));

    // build a stringList with all the param names
    for(medAbstractParameterL* layerParam : layersParams)
    {
        paramNames << layerParam->name();
    }

    QList<medAbstractParameterGroupL*> abstractGroupsList;
    QStringList selectedGroups;
    QStringList partiallySelectedGroups;

    auto layerGroupsList = medParameterGroupManagerL::instance()->layerGroups(this->identifier());
    for(medLayerParameterGroupL *layerGroup : layerGroupsList)
    {
        abstractGroupsList.append(layerGroup);
        bool selected = true;
        bool partiallySelected = false;

        QHashIterator<medAbstractLayeredView*, unsigned int> iterLayer(layers);
        while(iterLayer.hasNext())
        {
            iterLayer.next();
            if(!layerGroup->impactedLayers().contains(iterLayer.key(), iterLayer.key()->layerData(iterLayer.value())))
                selected = false;
            else
                partiallySelected = true;
        }
        if(selected)
        {
            selectedGroups << layerGroup->name();
        }
        else if(partiallySelected)
        {
            partiallySelectedGroups << layerGroup->name();
        }
    }

    d->layerLinkMenu->setAvailableParameters( paramNames.toSet().toList() );
    d->layerLinkMenu->setGroups(abstractGroupsList);
    d->layerLinkMenu->setSelectedGroups(selectedGroups);
    d->layerLinkMenu->setPartiallySelectedGroups(partiallySelectedGroups);

    linkLayout->addWidget(new QLabel(tr("Link layer properties: ")));
    linkLayout->addWidget(d->layerLinkMenu);

    return linkWidget;

}

void medAbstractWorkspaceLegacy::addViewstoGroup(QString group)
{
    medAbstractView* view = nullptr;

    medViewParameterGroupL *paramGroup = medParameterGroupManagerL::instance()->viewGroup(group, this->identifier());

    for(QUuid uuid : d->viewContainerStack->containersSelected())
    {
        medViewContainer *container = medViewContainerManager::instance()->container(uuid);
        view = container->view();

        if(view)
        {
            paramGroup->addImpactedView(view);
        }
    }
}

void medAbstractWorkspaceLegacy::removeViewsFromGroup(QString group)
{
    medAbstractView* view = nullptr;

    medViewParameterGroupL *paramGroup = medParameterGroupManagerL::instance()->viewGroup(group, this->identifier());

    for(QUuid uuid : d->viewContainerStack->containersSelected())
    {
        medViewContainer *container = medViewContainerManager::instance()->container(uuid);
        view = container->view();

        if(view)
        {
            paramGroup->removeImpactedView(view);
        }
    }
}

void medAbstractWorkspaceLegacy::addLayerstoGroup(QString group)
{
    medLayerParameterGroupL *paramGroup = medParameterGroupManagerL::instance()->layerGroup(group, this->identifier());
    medViewContainerManager *containerMng =  medViewContainerManager::instance();

    for(QListWidgetItem *item : d->selectedLayers)
    {
        int currentLayer = item->data(Qt::UserRole).toInt();

        QUuid containerUuid = d->containerForLayerWidgetsItem.value(item);
        medViewContainer *container = containerMng->container(containerUuid);
        medAbstractLayeredView *view = dynamic_cast<medAbstractLayeredView*>(container->view());

        paramGroup->addImpactedlayer(view, view->layerData(currentLayer));

        int row = d->layerListWidget->row(item);
        medPoolIndicatorL *indicator = d->poolIndicators[row];
        if(indicator)
        {
            indicator->addColorIndicator(paramGroup->color(), paramGroup->name());
            connect(paramGroup, SIGNAL(groupColorChanged(QColor,QColor)), indicator, SLOT(replaceColorIndicator(QColor,QColor)));
        }
    }
}

void medAbstractWorkspaceLegacy::removeLayersFromGroup(QString group)
{
    medLayerParameterGroupL *paramGroup = medParameterGroupManagerL::instance()->layerGroup(group, this->identifier());
    medViewContainerManager *containerMng =  medViewContainerManager::instance();

    for(QListWidgetItem *item : d->selectedLayers)
    {
        int currentLayer = item->data(Qt::UserRole).toInt();

        QUuid containerUuid = d->containerForLayerWidgetsItem.value(item);
        medViewContainer *container = containerMng->container(containerUuid);
        medAbstractLayeredView *view = dynamic_cast<medAbstractLayeredView*>(container->view());

        paramGroup->removeImpactedlayer(view, view->layerData(currentLayer));

        int row = d->layerListWidget->row(item);
        medPoolIndicatorL *indicator = d->poolIndicators[row];
        if(indicator)
            indicator->removeColorIndicator(paramGroup->color());
    }
}

void medAbstractWorkspaceLegacy::removeViewGroup(QString group)
{
    delete medParameterGroupManagerL::instance()->viewGroup(group, this->identifier());
}

void medAbstractWorkspaceLegacy::removeLayerGroup(QString group)
{
    delete medParameterGroupManagerL::instance()->layerGroup(group, this->identifier());
    updateLayersToolBox();
}

void medAbstractWorkspaceLegacy::addViewGroup(QString group)
{
    medViewParameterGroupL *newGroup = new medViewParameterGroupL(group, this);
    newGroup->setLinkAllParameters(true);
    newGroup->removeParameter("DataList");
    addViewGroup(newGroup);
}

void medAbstractWorkspaceLegacy::addViewGroup(medViewParameterGroupL * group)
{
    if(d->viewLinkMenu)
      d->viewLinkMenu->addGroup(group, true);
}

void medAbstractWorkspaceLegacy::addLayerGroup(QString group)
{
    medLayerParameterGroupL *newGroup = new medLayerParameterGroupL(group, this);
    newGroup->setLinkAllParameters(true);
    addLayerGroup(newGroup);
}

void medAbstractWorkspaceLegacy::addLayerGroup(medLayerParameterGroupL * group)
{
    if(d->layerLinkMenu)
      d->layerLinkMenu->addGroup(group, true);
}

void medAbstractWorkspaceLegacy::setViewGroups(QList<medViewParameterGroupL*> groups)
{
    Q_UNUSED(groups);
    for(medViewParameterGroupL* group : medParameterGroupManagerL::instance()->viewGroups(this->identifier()))
    {
        addViewGroup(group);
    }
}

void medAbstractWorkspaceLegacy::setLayerGroups(QList<medLayerParameterGroupL*> groups)
{
    for(medLayerParameterGroupL* group : groups)
    {
        addLayerGroup(group);
    }
}

void medAbstractWorkspaceLegacy::setInitialGroups()
{
    medViewParameterGroupL *viewGroup1 = new medViewParameterGroupL("View Group 1", this, this->identifier());
    viewGroup1->setLinkAllParameters(true);
    viewGroup1->removeParameter("DataList");

    medLayerParameterGroupL *layerGroup1 = new medLayerParameterGroupL("Layer Group 1", this, this->identifier());
    layerGroup1->setLinkAllParameters(true);
}

void medAbstractWorkspaceLegacy::changeViewGroupColor(QString group, QColor color)
{
    medViewParameterGroupL *paramGroup = medParameterGroupManagerL::instance()->viewGroup(group, this->identifier());
    paramGroup->setColor(color);
}

void medAbstractWorkspaceLegacy::changeLayerGroupColor(QString group, QColor color)
{
    medLayerParameterGroupL *paramGroup = medParameterGroupManagerL::instance()->layerGroup(group, this->identifier());
    paramGroup->setColor(color);
}

medProgressionStack* medAbstractWorkspaceLegacy::getProgressionStack()
{
    return d->progressionStack;
}
