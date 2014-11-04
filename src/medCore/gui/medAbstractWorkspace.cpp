/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractWorkspace.h>

#include <QUuid>
#include <QWidgetAction>

#include <medDataIndex.h>
#include <medSettingsManager.h>
#include <medToolBox.h>
#include <medViewContainer.h>
#include <medViewContainerManager.h>
#include <medTabbedViewContainers.h>
#include <medAbstractView.h>
#include <medAbstractLayeredView.h>
#include <medToolBoxHeader.h>
#include <medAbstractInteractor.h>
#include <medMetaDataKeys.h>
#include <medBoolParameter.h>
#include <medStringListParameter.h>
#include <medParameterPool.h>
#include <medAbstractParameter.h>
#include <medDataManager.h>
#include <medPoolIndicator.h>
#include <medLinkMenu.h>
#include <medListWidget.h>
#include <medAbstractParameterGroup.h>
#include <medViewParameterGroup.h>
#include <medLayerParameterGroup.h>
#include <medParameterGroupManager.h>


class medAbstractWorkspacePrivate
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
    medToolBox *layerListToolBox;
    medToolBox *interactorToolBox;
    medToolBox *navigatorToolBox;
    medToolBox *mouseInteractionToolBox;
    medListWidget* layerListWidget;

    QList<QListWidgetItem*> selectedLayers;
    QHash<int,medPoolIndicator*> poolIndicators;

    medParameterPool *temporaryPoolForInteractors;

    medLinkMenu *viewLinkMenu;
    medLinkMenu *layerLinkMenu;
};

medAbstractWorkspace::medAbstractWorkspace(QWidget *parent) : QObject(parent), d(new medAbstractWorkspacePrivate)
{
    d->parent = parent;

    d->selectionToolBox = new medToolBox;
    d->selectionToolBox->setTitle("Selection");
    d->selectionToolBox->header()->hide();
    d->selectionToolBox->hide();

    d->viewContainerStack = new medTabbedViewContainers(this, parent);

    connect(d->viewContainerStack, SIGNAL(containersSelectedChanged()), this, SLOT(updateNavigatorsToolBox()));

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

    d->layerListToolBox = new medToolBox;
    d->layerListToolBox->setTitle("Layer settings");
    d->layerListToolBox->hide();
    d->selectionToolBox->addWidget(d->layerListToolBox);

    d->interactorToolBox = new medToolBox;
    d->interactorToolBox->setTitle("Interactors");
    d->interactorToolBox->header()->hide();
    d->interactorToolBox->hide();
    d->selectionToolBox->addWidget(d->interactorToolBox);

    d->layerListToolBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    d->layerListWidget = NULL;

    d->temporaryPoolForInteractors = new medParameterPool(this);

    this->setUserLayerClosable(true);
    this->setUserLayerPoolable(true);
    this->setUserViewPoolable(true);

    d->viewLinkMenu = NULL;
    d->layerLinkMenu = NULL;
}

medAbstractWorkspace::~medAbstractWorkspace(void)
{
    delete d;
    d = NULL;
}

void medAbstractWorkspace::addToolBox(medToolBox *toolbox)
{
    d->toolBoxes.append(toolbox);
}

void medAbstractWorkspace::removeToolBox(medToolBox *toolbox)
{
    d->toolBoxes.removeOne(toolbox);
}

QList<medToolBox*> medAbstractWorkspace::toolBoxes() const
{
    return d->toolBoxes;
}

medToolBox* medAbstractWorkspace::selectionToolBox() const
{
    return d->selectionToolBox;
}

void medAbstractWorkspace::setDatabaseVisibility(bool visibility)
{
    d->databaseVisibility = visibility;
    emit databaseVisibilitySetted(visibility);
}

bool medAbstractWorkspace::isDatabaseVisible(void) const
{
    return d->databaseVisibility;
}

medTabbedViewContainers* medAbstractWorkspace::stackedViewContainers() const
{
    return d->viewContainerStack;
}

void medAbstractWorkspace::clear()
{
    this->setupViewContainerStack();
    this->clearWorkspaceToolBoxes();
    return;
}

void medAbstractWorkspace::setToolBoxesVisibility (bool value)
{
    d->toolBoxesVisibility = value;
    emit toolBoxesVisibilitySetted(value);
}

bool medAbstractWorkspace::areToolBoxesVisible() const
{
    return d->toolBoxesVisibility;
}

void medAbstractWorkspace::clearWorkspaceToolBoxes()
{
    foreach(medToolBox* tb,d->toolBoxes)
        tb->clear();
}

void medAbstractWorkspace::addNewTab()
{
    QString tabName = QString("%1 %2").arg(this->name()).arg(d->viewContainerStack->count());
    d->viewContainerStack->addContainerInTab(tabName);
}

void medAbstractWorkspace::updateNavigatorsToolBox()
{
    d->navigatorToolBox->clear();

    medAbstractView* view = NULL;
    QList<QWidget*>  navigators;
    QStringList viewType;

    foreach(QUuid uuid, d->viewContainerStack->containersSelected())
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
    foreach(QWidget* navigator, navigators)
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

void medAbstractWorkspace::updateMouseInteractionToolBox()
{
    d->mouseInteractionToolBox->clear();

    QList<QWidget*>  navigators;
    QStringList viewType;
    foreach(QUuid uuid, d->viewContainerStack->containersSelected())
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
    foreach(QWidget* navigator, navigators)
    {
        d->mouseInteractionToolBox->addWidget(navigator);
        navigator->show();
    }
}

void medAbstractWorkspace::updateLayersToolBox()
{
    d->layerListToolBox->clear();
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
    connect(d->layerListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(updateInteractorsToolBox()));

    foreach(QUuid uuid, d->viewContainerStack->containersSelected())
    {
        // fill the layer widget
        medViewContainer *container = medViewContainerManager::instance()->container(uuid);
        medAbstractLayeredView* layeredView = dynamic_cast<medAbstractLayeredView*>(container->view());
        if(layeredView)
        {
            if(d->layerListWidget->count() != 0)
            {
                // add an empty widget to separate layers from different views
                QListWidgetItem * item = new QListWidgetItem;
                item->setSizeHint(QSize(10, 10));
                item->setFlags(Qt::NoItemFlags);
                d->layerListWidget->addItem(item);
            }

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
                layout->setContentsMargins(0,0,10,0);

                medBoolParameter* visibilityParam = dynamic_cast<medBoolParameter*>(layeredView->visibilityParameter(layer));
                QPushButton* thumbnailButton = visibilityParam->getPushButton();

                QFont myFont;
                QFontMetrics fm(myFont);
                //TODO: could be nice to elide according to current width (update when resize)
                QString text = fm.elidedText(name, Qt::ElideRight, 100);
                QLabel *layerName = new QLabel("<font color='Black'>"+text+"</font>", layerWidget);
                layerName->setToolTip(name);

                layout->addWidget(thumbnailButton);
                layout->addWidget(layerName);
                layout->addStretch();
                foreach (medAbstractInteractor *interactor, layeredView->layerInteractors(layer))
                {
                    if(interactor->layerWidget())
                        layout->addWidget(interactor->layerWidget());
                }

                if(d->userLayerPoolable)
                {
                    medPoolIndicator *poolIndicator = new medPoolIndicator;
                    layout->addWidget(poolIndicator);
                    d->poolIndicators.insert(d->layerListWidget->count(), poolIndicator);

                    QList<medLayerParameterGroup*> layerGroups = medParameterGroupManager::instance()->layerGroups(layeredView, data);
                    foreach(medLayerParameterGroup *layerGroup, layerGroups)
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
        }
    }
    // add the layer widgets
    d->layerListToolBox->show();
    d->layerListToolBox->addWidget(d->layerListWidget);

    d->layerListWidget->show();

    this->updateInteractorsToolBox();
}

void medAbstractWorkspace::changeCurrentLayer(int row)
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

void medAbstractWorkspace::updateInteractorsToolBox()
{
    medViewContainerManager *containerMng =  medViewContainerManager::instance();
    foreach(QUuid uuid, d->viewContainerStack->containersSelected())
    {
        containerMng->container(uuid)->highlight();
    }
    d->interactorToolBox->hide();
    d->interactorToolBox->clear();

    if(!d->layerListWidget)
        return;

    // Hack to always have something selected in the layerListWidget
    // TODO: try to find a better solution...
    if(d->layerListWidget->selectedItems().count() == 0)
    {
        foreach(QListWidgetItem* item, d->selectedLayers)
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
        foreach(QListWidgetItem* item, d->layerListWidget->selectedItems())
        {
            QUuid containerUuid = d->containerForLayerWidgetsItem.value(item);
            medViewContainer *container = containerMng->container(containerUuid);
            container->highlight("#FF8844");
        }

        buildTemporaryPool();

        return;
    }

    foreach (medAbstractInteractor* interactor, view->layerInteractors(currentLayer))
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


void medAbstractWorkspace::removeLayer()
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

    this->updateLayersToolBox();
}


void medAbstractWorkspace::buildTemporaryPool()
{
    medViewContainerManager *containerMng =  medViewContainerManager::instance();
    d->temporaryPoolForInteractors->clear();

    foreach(QListWidgetItem* item, d->layerListWidget->selectedItems())
    {
        QUuid containerUuid = d->containerForLayerWidgetsItem.value(item);
        medViewContainer *container = containerMng->container(containerUuid);
        int layer = item->data(Qt::UserRole).toInt();

        medAbstractLayeredView *view = dynamic_cast<medAbstractLayeredView*>(container->view());

        foreach (medAbstractInteractor* interactor, view->layerInteractors(layer))
        {
            d->temporaryPoolForInteractors->append(interactor->linkableParameters());
        }
    }
}

void medAbstractWorkspace::open(const medDataIndex &index)
{
    QList<QUuid> containersSelected = d->viewContainerStack->containersSelected();
    if(containersSelected.size() != 1)
        return;

    medViewContainer *container = medViewContainerManager::instance()->container(containersSelected.first());
    if(index.isValidForSeries())
        container->addData(medDataManager::instance()->retrieveData(index));
}

void medAbstractWorkspace::setUserLayerPoolable(bool poolable)
{
    d->userLayerPoolable = poolable;
    this->updateLayersToolBox();
}

void medAbstractWorkspace::setUserViewPoolable(bool poolable)
{
    d->userViewPoolable = poolable;
    this->updateNavigatorsToolBox();
}

void medAbstractWorkspace::setUserLayerClosable(bool Closable)
{
    d->userLayerClosable = Closable;
    this->updateLayersToolBox();
}

bool medAbstractWorkspace::isUserLayerPoolable() const
{
    return d->userLayerPoolable;
}

bool medAbstractWorkspace::isUserViewPoolable() const
{
    return d->userViewPoolable;
}

bool medAbstractWorkspace::isUserLayerClosable() const
{
    return d->userLayerClosable;
}


QWidget* medAbstractWorkspace::buildViewLinkMenu()
{
    QWidget *linkWidget = new QWidget;
    QHBoxLayout* linkLayout = new QHBoxLayout(linkWidget);

    d->viewLinkMenu = new medLinkMenu(linkWidget);
    connect(d->viewLinkMenu, SIGNAL(groupChecked(QString)), this, SLOT(addViewstoGroup(QString)));
    connect(d->viewLinkMenu, SIGNAL(groupUnchecked(QString)), this, SLOT(removeViewsFromGroup(QString)));
    connect(d->viewLinkMenu, SIGNAL(groupCreated(QString)), this, SLOT(addViewGroup(QString)));
    connect(d->viewLinkMenu, SIGNAL(groupDeleted(QString)), this, SLOT(removeViewGroup(QString)));
    connect(d->viewLinkMenu, SIGNAL(groupColorChangeRequest(QString,QColor)), this, SLOT(changeViewGroupColor(QString, QColor)));

    medAbstractView* view = NULL;
    QStringList viewType;
    QList<medAbstractView*> views;
    QList<medAbstractParameter*>  viewsParams;

    foreach(QUuid uuid, d->viewContainerStack->containersSelected())
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
    foreach(medAbstractParameter* viewParam, viewsParams)
        paramNames << viewParam->name();

    QList<medAbstractParameterGroup*>  groups;
    QStringList selectedGroups;
    QStringList partiallySelectedGroups;

    //TODO: to review
    foreach(medViewParameterGroup *viewGroup, medParameterGroupManager::instance()->viewGroups(this->identifier()))
    {
        groups.append(viewGroup);
        bool selected = true;
        bool partiallySelected = false;

        foreach(medAbstractView *view, views)
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
    d->viewLinkMenu->setGroups(groups);
    d->viewLinkMenu->setSelectedGroups(selectedGroups);
    d->viewLinkMenu->setPartiallySelectedGroups(partiallySelectedGroups);

    linkLayout->addWidget(new QLabel(tr("Link view properties: ")));
    linkLayout->addWidget(d->viewLinkMenu);

    return linkWidget;
}


QWidget* medAbstractWorkspace::buildLayerLinkMenu(QList<QListWidgetItem*> selectedlayers)
{
    QStringList paramNames;
    QList<medAbstractParameter*> layersParams;
    QMultiHash<medAbstractLayeredView*, unsigned int> layers;
    medViewContainerManager *containerMng =  medViewContainerManager::instance();

    foreach(QListWidgetItem *item, selectedlayers)
    {
        int currentLayer = item->data(Qt::UserRole).toInt();
        QList<QString> interactorsIdentifier;

        QUuid containerUuid = d->containerForLayerWidgetsItem.value(item);
        medViewContainer *container = containerMng->container(containerUuid);
        medAbstractLayeredView *view = dynamic_cast<medAbstractLayeredView*>(container->view());

        foreach (medAbstractInteractor* interactor, view->layerInteractors(currentLayer))
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

    d->layerLinkMenu = new medLinkMenu(linkWidget);
    connect(d->layerLinkMenu, SIGNAL(groupChecked(QString)), this, SLOT(addLayerstoGroup(QString)));
    connect(d->layerLinkMenu, SIGNAL(groupUnchecked(QString)), this, SLOT(removeLayersFromGroup(QString)));

    connect(d->layerLinkMenu, SIGNAL(groupCreated(QString)), this, SLOT(addLayerGroup(QString)));
    connect(d->layerLinkMenu, SIGNAL(groupDeleted(QString)), this, SLOT(removeLayerGroup(QString)));
    connect(d->layerLinkMenu, SIGNAL(groupColorChangeRequest(QString,QColor)), this, SLOT(changeLayerGroupColor(QString, QColor)));

    // build a stringList with all the param names
    foreach(medAbstractParameter* layerParam, layersParams)
        paramNames << layerParam->name();

    QList<medAbstractParameterGroup*>  groups;
    QStringList selectedGroups;
    QStringList partiallySelectedGroups;

    foreach(medLayerParameterGroup *layerGroup, medParameterGroupManager::instance()->layerGroups(this->identifier()))
    {
        groups.append(layerGroup);
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
            selectedGroups << layerGroup->name();
        else if(partiallySelected)
            partiallySelectedGroups << layerGroup->name();
    }

    d->layerLinkMenu->setAvailableParameters( paramNames.toSet().toList() );
    d->layerLinkMenu->setGroups(groups);
    d->layerLinkMenu->setSelectedGroups(selectedGroups);
    d->layerLinkMenu->setPartiallySelectedGroups(partiallySelectedGroups);

    linkLayout->addWidget(new QLabel(tr("Link layer properties: ")));
    linkLayout->addWidget(d->layerLinkMenu);

    return linkWidget;

}

void medAbstractWorkspace::addViewstoGroup(QString group)
{
    medAbstractView* view = NULL;

    medViewParameterGroup *paramGroup = medParameterGroupManager::instance()->viewGroup(group, this->identifier());

    foreach(QUuid uuid, d->viewContainerStack->containersSelected())
    {
        medViewContainer *container = medViewContainerManager::instance()->container(uuid);
        view = container->view();

        if(!view)
            continue;

        paramGroup->addImpactedView(view);
    }
}

void medAbstractWorkspace::removeViewsFromGroup(QString group)
{
    medAbstractView* view = NULL;

    medViewParameterGroup *paramGroup = medParameterGroupManager::instance()->viewGroup(group, this->identifier());

    foreach(QUuid uuid, d->viewContainerStack->containersSelected())
    {
        medViewContainer *container = medViewContainerManager::instance()->container(uuid);
        view = container->view();

        if(!view)
            continue;

        paramGroup->removeImpactedView(view);
    }
}

void medAbstractWorkspace::addLayerstoGroup(QString group)
{
    medLayerParameterGroup *paramGroup = medParameterGroupManager::instance()->layerGroup(group, this->identifier());
    medViewContainerManager *containerMng =  medViewContainerManager::instance();

    foreach(QListWidgetItem *item, d->selectedLayers)
    {
        int currentLayer = item->data(Qt::UserRole).toInt();

        QUuid containerUuid = d->containerForLayerWidgetsItem.value(item);
        medViewContainer *container = containerMng->container(containerUuid);
        medAbstractLayeredView *view = dynamic_cast<medAbstractLayeredView*>(container->view());

        paramGroup->addImpactedlayer(view, view->layerData(currentLayer));

        int row = d->layerListWidget->row(item);
        medPoolIndicator *indicator = d->poolIndicators[row];
        if(indicator)
        {
            indicator->addColorIndicator(paramGroup->color(), paramGroup->name());
            connect(paramGroup, SIGNAL(groupColorChanged(QColor,QColor)), indicator, SLOT(replaceColorIndicator(QColor,QColor)));
        }
    }
}

void medAbstractWorkspace::removeLayersFromGroup(QString group)
{
    medLayerParameterGroup *paramGroup = medParameterGroupManager::instance()->layerGroup(group, this->identifier());
    medViewContainerManager *containerMng =  medViewContainerManager::instance();

    foreach(QListWidgetItem *item, d->selectedLayers)
    {
        int currentLayer = item->data(Qt::UserRole).toInt();

        QUuid containerUuid = d->containerForLayerWidgetsItem.value(item);
        medViewContainer *container = containerMng->container(containerUuid);
        medAbstractLayeredView *view = dynamic_cast<medAbstractLayeredView*>(container->view());

        paramGroup->removeImpactedlayer(view, view->layerData(currentLayer));

        int row = d->layerListWidget->row(item);
        medPoolIndicator *indicator = d->poolIndicators[row];
        if(indicator)
            indicator->removeColorIndicator(paramGroup->color());
    }
}

void medAbstractWorkspace::removeViewGroup(QString group)
{
    delete medParameterGroupManager::instance()->viewGroup(group, this->identifier());
}

void medAbstractWorkspace::removeLayerGroup(QString group)
{
    delete medParameterGroupManager::instance()->layerGroup(group, this->identifier());
    updateLayersToolBox();
}

void medAbstractWorkspace::addViewGroup(QString group)
{
    medViewParameterGroup *newGroup = new medViewParameterGroup(group, this);
    newGroup->setLinkAllParameters(true);
    newGroup->removeParameter("DataList");
    addViewGroup(newGroup);
}

void medAbstractWorkspace::addViewGroup(medViewParameterGroup * group)
{
    if(d->viewLinkMenu)
      d->viewLinkMenu->addGroup(group, true);
}

void medAbstractWorkspace::addLayerGroup(QString group)
{
    medLayerParameterGroup *newGroup = new medLayerParameterGroup(group, this);
    newGroup->setLinkAllParameters(true);
    addLayerGroup(newGroup);
}

void medAbstractWorkspace::addLayerGroup(medLayerParameterGroup * group)
{
    if(d->layerLinkMenu)
      d->layerLinkMenu->addGroup(group, true);
}

void medAbstractWorkspace::setViewGroups(QList<medViewParameterGroup*> groups)
{
    foreach(medViewParameterGroup* group, medParameterGroupManager::instance()->viewGroups(this->identifier()))
        addViewGroup(group);
}

void medAbstractWorkspace::setLayerGroups(QList<medLayerParameterGroup*> groups)
{
    foreach(medLayerParameterGroup* group, groups)
        addLayerGroup(group);
}

void medAbstractWorkspace::changeViewGroupColor(QString group, QColor color)
{
    medViewParameterGroup *paramGroup = medParameterGroupManager::instance()->viewGroup(group, this->identifier());
    paramGroup->setColor(color);
}

void medAbstractWorkspace::changeLayerGroupColor(QString group, QColor color)
{
    medLayerParameterGroup *paramGroup = medParameterGroupManager::instance()->layerGroup(group, this->identifier());
    paramGroup->setColor(color);
}
