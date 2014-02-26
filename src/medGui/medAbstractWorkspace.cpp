/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractWorkspace.h>

#include <QUuid>

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

class medAbstractWorkspacePrivate
{
public:
    QWidget *parent;

    bool databaseVisibility;
    bool toolBoxesVisibility;

    medTabbedViewContainers * viewContainerStack;
    QHash <QListWidgetItem*, QUuid> containerForLayerWidgetsItem;
    QHash <int, QUuid> containerForLayerWidgetRow;
    QHash <QUuid, QPair<int, int> > layersRangeInRowsForContainer;
    QHash <QPushButton*, int> rowForVisibilityLayerButton;
    QHash <QPushButton*, int> rowForRemoveLayerButton;


    QList<medToolBox*> workspaceToolBoxes;
    medToolBox *selectionToolBox;
    medToolBox *layerListToolBox;
    medToolBox *interactorToolBox;
    medToolBox *navigatorToolBox;
    QListWidget* layerListWidget;

    QList<QListWidgetItem*> selectedLayers;
};

medAbstractWorkspace::medAbstractWorkspace(QWidget *parent) : QObject(parent), d(new medAbstractWorkspacePrivate)
{
    d->parent = parent;

    d->selectionToolBox = new medToolBox;
    d->selectionToolBox->setTitle("Selection");
    d->selectionToolBox->header()->hide();
    d->selectionToolBox->hide();

    d->viewContainerStack = new medTabbedViewContainers(parent);
    connect(d->viewContainerStack, SIGNAL(containersSelectedChanged()), this, SLOT(updateNavigatorsToolBox()));
    connect(d->viewContainerStack, SIGNAL(currentLayerChanged()), this, SLOT(updateInteractorsToolBox()));

    d->databaseVisibility = true;
    d->toolBoxesVisibility = true;

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

    d->layerListWidget = NULL;
}

medAbstractWorkspace::~medAbstractWorkspace(void)
{
    delete d;
    d = NULL;
}

void medAbstractWorkspace::addWorkspaceToolBox(medToolBox *toolbox)
{
    d->workspaceToolBoxes.append(toolbox);
}

void medAbstractWorkspace::removeWorkspaceToolBox(medToolBox *toolbox)
{
    d->workspaceToolBoxes.removeOne(toolbox);
}

QList<medToolBox*> medAbstractWorkspace::workspaceToolBoxes() const
{
    return d->workspaceToolBoxes;
}

medToolBox* medAbstractWorkspace::selectionToolBox() const
{
    return d->selectionToolBox;
}

void medAbstractWorkspace::setDatabaseVisibility(bool visibility)
{
    d->databaseVisibility = visibility;
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
}

bool medAbstractWorkspace::areToolBoxesVisible() const
{
    return d->toolBoxesVisibility;
}


void medAbstractWorkspace::clearWorkspaceToolBoxes()
{
    foreach(medToolBox* tb,d->workspaceToolBoxes)
        tb->clear();
}

void medAbstractWorkspace::addNewTab()
{
    d->viewContainerStack->addContainerInTab(this->identifier());
}

void medAbstractWorkspace::updateNavigatorsToolBox()
{
    d->navigatorToolBox->clear();

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

    // update the layer toolbox according to the selected containers
    this->updateLayersToolBox();
}

void medAbstractWorkspace::updateLayersToolBox()
{
    d->layerListToolBox->clear();
    d->layersRangeInRowsForContainer.clear();
    d->containerForLayerWidgetsItem.clear();
    d->containerForLayerWidgetRow.clear();
    d->rowForRemoveLayerButton.clear();
    d->rowForVisibilityLayerButton.clear();
    d->selectedLayers.clear();

    delete d->layerListWidget;
    d->layerListWidget = new QListWidget;
    d->layerListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    d->layerListWidget->setFocusPolicy(Qt::NoFocus);

    connect(d->layerListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(changeCurrentLayer(int)));
    connect(d->layerListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(updateInteractorsToolBox()));

    foreach(QUuid uuid, d->viewContainerStack->containersSelected())
    {
        // fill the layer widget
        medViewContainer *container = medViewContainerManager::instance()->container(uuid);
        medAbstractLayeredView* layerdView = dynamic_cast<medAbstractLayeredView*>(container->view());
        if(layerdView)
        {
            if(d->layerListWidget->count() != 0)
            {
                // add an empty widget to separate layers from different views
                QListWidgetItem * item = new QListWidgetItem;
                item->setSizeHint(QSize(10, 10));
                item->setFlags(Qt::NoItemFlags);
                d->layerListWidget->addItem(item);
            }
            int firstLayerIndex = d->layerListWidget->count();

            for(int layer = 0; layer < layerdView->layersCount(); ++layer)
            {
                QWidget *layerWidget = new QWidget;
                layerWidget->setObjectName("layerWidget");

                medAbstractData *data = layerdView->data(layer);
                QString thumbPath = medMetaDataKeys::SeriesThumbnail.getFirstValue(data,":icons/layer.png");
                QString name = medMetaDataKeys::SeriesDescription.getFirstValue(data,"<i>no name</i>");

                QHBoxLayout* layout = new QHBoxLayout(layerWidget);
                layout->setContentsMargins(0,0,10,0);

                QPushButton* thumbnailButton = new QPushButton(layerWidget);
                d->rowForVisibilityLayerButton.insert(thumbnailButton, d->layerListWidget->count());
                QIcon thumbnailIcon;
                // Set the off icon to the greyed out version of the regular icon
                thumbnailIcon.addPixmap(QPixmap(thumbPath), QIcon::Normal, QIcon::On);
                QStyleOption opt(0);
                opt.palette = QApplication::palette();
                QPixmap pix = QApplication::style()->generatedIconPixmap(QIcon::Disabled, QPixmap(thumbPath), &opt);
                thumbnailIcon.addPixmap(pix, QIcon::Normal, QIcon::Off);
                thumbnailButton->setFocusPolicy(Qt::NoFocus);
                thumbnailButton->setIcon(thumbnailIcon);
                thumbnailButton->setIconSize(QSize(22,22));
                thumbnailButton->setCheckable(true);
                thumbnailButton->setChecked(true);
                thumbnailButton->setFlat(true);

                QLabel *layerName = new QLabel(name, layerWidget);

                QPushButton *removeButton = new QPushButton;
                d->rowForRemoveLayerButton.insert(removeButton, d->layerListWidget->count());
                removeButton->setIcon(QIcon(":/icons/cross.svg"));

                layout->addWidget(thumbnailButton);
                layout->addWidget(layerName);
                layout->addStretch();
                foreach (medAbstractInteractor *interactor, layerdView->interactors(layer))
                    layout->addWidget(interactor->layerWidget());

                layout->addWidget(removeButton);

                connect(thumbnailButton, SIGNAL(clicked(bool)), this, SLOT(setLayerVisibility(bool)));
                connect(removeButton, SIGNAL(clicked()), this, SLOT(removeLayer()));

                layerWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
                layerWidget->resize(d->selectionToolBox->width(), 25);

                QListWidgetItem * item = new QListWidgetItem;
                d->containerForLayerWidgetsItem.insert(item, uuid);
                d->containerForLayerWidgetRow.insert(d->layerListWidget->count(), uuid);
                item->setSizeHint(QSize(layerWidget->width(), 25));
                d->layerListWidget->addItem(item);
                d->layerListWidget->setItemWidget(item, layerWidget);
                layerWidget->setFocusPolicy(Qt::NoFocus);
            }

            d->layersRangeInRowsForContainer.insert(uuid, QPair<int, int>(firstLayerIndex, d->layerListWidget->count()));
            d->layerListWidget->setCurrentRow(firstLayerIndex + layerdView->currentLayer());
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
    QUuid uuid = d->containerForLayerWidgetRow.value(row);
    medViewContainer* container = medViewContainerManager::instance()->container(uuid);
    if(!container)
        return;

    medAbstractLayeredView *layeredView = dynamic_cast<medAbstractLayeredView*>(container->view());
    if(!layeredView)
        return;

    int currentLayer = row - d->layersRangeInRowsForContainer.value(uuid).first;
    layeredView->setCurrentLayer(currentLayer);

    d->selectedLayers = d->layerListWidget->selectedItems();

    this->updateInteractorsToolBox();
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

    // If we have a multiSelection we just want to colorise the container that have at least one layer selected.
    if(d->layerListWidget->selectedItems().size() > 1)
    {
        foreach(QListWidgetItem* item, d->layerListWidget->selectedItems())
        {
            QUuid containerUuid = d->containerForLayerWidgetsItem.value(item);
            medViewContainer *container = containerMng->container(containerUuid);
            container->highlight("#FF6622");
        }
        return;
    }

    QListWidgetItem* item = d->layerListWidget->currentItem();
    if(!item)
        return;

    QList<QString> interactorsIdentifier;
    QUuid containerUuid = d->containerForLayerWidgetsItem.value(item);
    medViewContainer *container = containerMng->container(containerUuid);
    container->highlight("#FF6622");

    medAbstractLayeredView *view = dynamic_cast<medAbstractLayeredView*>(container->view());

    foreach (medAbstractInteractor* interactor, view->currentInteractor())
    {
        QString interactorIdentifier = interactor->identifier();
        if(!interactorsIdentifier.contains(interactorIdentifier))
        {
            interactorsIdentifier << interactorIdentifier;
            d->interactorToolBox->addWidget(interactor->toolBoxWidget());
            interactor->toolBoxWidget()->show();
        }
    }
    d->interactorToolBox->show();
}

void medAbstractWorkspace::setLayerVisibility(bool visibility)
{
    QPushButton *button = dynamic_cast<QPushButton*>(this->sender());
    if(!button)
        return;

    int row = d->rowForVisibilityLayerButton.value(button);
    QUuid containerUuid = d->containerForLayerWidgetRow.value(row);
    int layer = row - d->layersRangeInRowsForContainer.value(containerUuid).first;

    medAbstractLayeredView *layerView = dynamic_cast<medAbstractLayeredView *>(medViewContainerManager::instance()->container(containerUuid)->view());
    if(!layerView)
        return;

    layerView->setVisibility(visibility, layer);
}


void medAbstractWorkspace::removeLayer()
{
    QPushButton *button = dynamic_cast<QPushButton*>(this->sender());
    if(!button)
        return;

    int row = d->rowForRemoveLayerButton.value(button);
    QUuid containerUuid = d->containerForLayerWidgetRow.value(row);
    int layer = row - d->layersRangeInRowsForContainer.value(containerUuid).first;

    medAbstractLayeredView *layerView = dynamic_cast<medAbstractLayeredView *>(medViewContainerManager::instance()->container(containerUuid)->view());
    if(!layerView)
        return;

    layerView->removeLayer(layer);
    this->updateLayersToolBox();
}
