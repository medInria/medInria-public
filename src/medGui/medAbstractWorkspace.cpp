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

class medAbstractWorkspacePrivate
{
public:
    QWidget *parent;

    bool databaseVisibility;
    bool toolBoxesVisibility;

    medTabbedViewContainers * viewContainerStack;
    QHash <QListWidgetItem*, QUuid> containerForLayerWidgets;
    QHash <QUuid, QPair<int, int> > indexLayerWidgetForContainer;

    QList<medToolBox*> workspaceToolBoxes;
    medToolBox *selectionToolBox;
    medToolBox *layerListToolBox;
    medToolBox *interactorToolBox;
    medToolBox *navigatorToolBox;
    QListWidget* layerListWidget;
};

medAbstractWorkspace::medAbstractWorkspace(QWidget *parent) : QObject(parent), d(new medAbstractWorkspacePrivate)
{
    d->parent = parent;

    d->selectionToolBox = new medToolBox;
    d->selectionToolBox->setTitle("Selection");
    d->selectionToolBox->header()->hide();
    d->selectionToolBox->hide();

    d->viewContainerStack = new medTabbedViewContainers(parent);
    connect(d->viewContainerStack, SIGNAL(selectionChanged()), this, SLOT(updateForContainerSelection()));

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

    d->layerListWidget = new QListWidget;
    d->layerListWidget->hide();
    d->layerListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
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

void medAbstractWorkspace::updateForContainerSelection()
{
    d->navigatorToolBox->clear();

    QList<QWidget*>  navigators;
    QStringList viewType;
    foreach(QUuid uuid, d->viewContainerStack->containersSelected())
    {
        medViewContainer *container = medViewContainerManager::instance()->container(uuid);
        // update the toolbox when the content of the view change
        connect(container, SIGNAL(viewChanged()), this, SLOT(updateForContainerSelection()));
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

    this->updateForLayeredViewContents();

    emit selectionChanged();
}


void medAbstractWorkspace::updateForLayeredViewContents()
{
    d->layerListToolBox->clear();
    d->indexLayerWidgetForContainer.clear();

    // unparent the layers widget before clearing d->layerListWidget;
    for(int row = 0; row < d->layerListWidget->count(); ++row)
    {
        QWidget *widget = d->layerListWidget->itemWidget(d->layerListWidget->item(row));
        if(widget)
            widget->setParent(NULL);
    }


    delete d->layerListWidget;
    d->layerListWidget = new QListWidget;
    d->layerListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(d->layerListWidget, SIGNAL(itemSelectionChanged ()), this, SLOT(updateForLayerSelection()));

    d->layerListToolBox->clear();
    foreach(QUuid uuid, d->viewContainerStack->containersSelected())
    {
        // fill the layer widget
        medViewContainer *container = medViewContainerManager::instance()->container(uuid);
        medAbstractLayeredView* layerdView = dynamic_cast<medAbstractLayeredView*>(container->view());
        if(layerdView)
        {
            connect(layerdView, SIGNAL(layerAdded(int)), this, SLOT(updateForLayeredViewContents()), Qt::UniqueConnection);
            connect(layerdView, SIGNAL(layerRemoved(int)), this, SLOT(updateForLayeredViewContents()), Qt::UniqueConnection);

            int firstLayerIndex = d->layerListWidget->count();
            if(d->layerListWidget->count() == 0)
            {
                QListWidgetItem * item = new QListWidgetItem;
                item->setSizeHint(QSize(10, 10));
                item->setFlags(Qt::NoItemFlags);
                d->layerListWidget->addItem(item);
            }
            connect(container, SIGNAL(viewRemoved()), this, SLOT(updateForEmptyContainer()), Qt::UniqueConnection);

            foreach(QWidget *layerWidget, layerdView->layerWidgets())
            {
                layerWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
                layerWidget->resize(d->selectionToolBox->width(), 25);

                QListWidgetItem * item = new QListWidgetItem;
                d->containerForLayerWidgets.insert(item, uuid);
                item->setSizeHint(QSize(layerWidget->width(), 25));
                d->layerListWidget->addItem(item);
                d->layerListWidget->setItemWidget(item, layerWidget);
            }
            d->indexLayerWidgetForContainer.insert(uuid, QPair<int, int>(firstLayerIndex, d->layerListWidget->count()));
        }
    }
    // add the layer widgets
    d->layerListToolBox->show();
    d->layerListToolBox->addWidget(d->layerListWidget);
    d->layerListWidget->show();
}

void medAbstractWorkspace::updateForEmptyContainer()
{
    medViewContainer *container = dynamic_cast<medViewContainer *>(this->sender());
    if(!container)
        return;

    int firstLayer = d->indexLayerWidgetForContainer.value(container->uuid()).first;
    int layerCount = d->indexLayerWidgetForContainer.value(container->uuid()).second;
    for(int row = firstLayer; row < layerCount; ++row)
        delete d->layerListWidget->itemWidget(d->layerListWidget->item(row));
}

void medAbstractWorkspace::updateForLayerSelection()
{
    medViewContainerManager *containerMng =  medViewContainerManager::instance();
    foreach(QUuid uuid, d->viewContainerStack->containersSelected())
    {
        containerMng->container(uuid)->highlight();
    }
    d->interactorToolBox->hide();
    d->interactorToolBox->clear();

    QList<QString> interactorsIdentifier;

    QGroupBox *imageBox = new QGroupBox("IMAGE");
    QVBoxLayout *imageLayout = new QVBoxLayout(imageBox);
    QGroupBox *meshBox = new QGroupBox("MESH");
    QVBoxLayout *meshLayout = new QVBoxLayout(meshBox);
    QGroupBox *unknowBox = new QGroupBox("UNKNOWN");
    QVBoxLayout *unknowLayout = new QVBoxLayout(unknowBox);

    foreach (QListWidgetItem* item, d->layerListWidget->selectedItems())
    {
        medViewContainer *container = containerMng->container(d->containerForLayerWidgets.value(item));
        container->highlight("#FF6622");
        medAbstractLayeredView *view = dynamic_cast<medAbstractLayeredView*>(container->view());

        foreach (medAbstractInteractor* interactor, view->interactors())
        {

            QString interactorIdentifier = interactor->identifier();
            if(!interactorsIdentifier.contains(interactorIdentifier))
            {
                interactorsIdentifier << interactorIdentifier;

                if(interactorIdentifier.contains("itk", Qt::CaseInsensitive))
                    imageLayout->addWidget(interactor->toolBoxWidget());
                else if(interactorIdentifier.contains("vtk", Qt::CaseInsensitive))
                    meshLayout->addWidget(interactor->toolBoxWidget());
                else
                    unknowLayout->addWidget(interactor->toolBoxWidget());
            }
        }
    }

    d->interactorToolBox->show();
    if(!imageLayout->isEmpty())
    {
        d->interactorToolBox->addWidget(imageBox);
        imageBox->show();
    }
    if(!meshLayout->isEmpty())
    {
        d->interactorToolBox->addWidget(meshBox);
        meshBox->show();
    }
    if(!unknowLayout->isEmpty())
    {
        d->interactorToolBox->addWidget(unknowBox);
        unknowBox->show();
    }

    emit selectionChanged();
}
