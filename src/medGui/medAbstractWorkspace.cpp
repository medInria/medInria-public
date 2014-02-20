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
#include <medViewSettingsToolBox.h>
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

    d->selectionToolBox = new medViewSettingsToolBox;
    d->selectionToolBox->setTitle("Selection");
    d->selectionToolBox->header()->hide();
    d->selectionToolBox->hide();

    d->viewContainerStack = new medTabbedViewContainers(parent);
    connect(d->viewContainerStack, SIGNAL(newContainer(QUuid)), this, SLOT(addNewContainer(QUuid)));
    connect(d->viewContainerStack, SIGNAL(selectionChanged()), this, SLOT(updateForContainerSelection()));
    connect(medViewContainerManager::instance(), SIGNAL(containerAboutToBeDestroyed(QUuid)), this, SLOT(removeContainer(QUuid)));

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
    // unparent the layers widget before clearing d->layerListWidget;
    for(int row = 0; row < d->layerListWidget->count(); ++row)
        d->layerListWidget->itemWidget(d->layerListWidget->item(row))->setParent(NULL);

    delete d->layerListWidget;
    d->layerListWidget = new QListWidget;
    d->layerListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(d->layerListWidget, SIGNAL(itemSelectionChanged ()), this, SLOT(updateForLayerSelection()));

    d->navigatorToolBox->clear();
    d->layerListToolBox->clear();

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
            qDebug() << view->navigatorWidget();
        }


        // fill the layer widget
        medAbstractLayeredView* layerdView = dynamic_cast<medAbstractLayeredView*>(view);
        if(layerdView)
        {
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
        }
    }
    // add the navigators widgets
    d->navigatorToolBox->show();
    foreach(QWidget* navigator, navigators)
    {
        d->navigatorToolBox->addWidget(navigator);
        navigator->show();
    }

    // add the layer widgets
    d->layerListToolBox->show();
    d->layerListToolBox->addWidget(d->layerListWidget);
    d->layerListWidget->show();

    emit selectionChanged();
}


void medAbstractWorkspace::updateForLayerSelection()
{
    qDebug()  << "update layer selection";
    medViewContainerManager *containerMng =  medViewContainerManager::instance();
    foreach(QUuid uuid, d->viewContainerStack->containersSelected())
    {
        containerMng->container(uuid)->highlight();
    }

    d->interactorToolBox->clear();

    QList<QWidget*> interactorsWidget;
    QList<QString> interactorsIdentifier;

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
                interactorsWidget << interactor->toolBoxWidget();
            }
        }
    }

    d->interactorToolBox->show();
    foreach(QWidget* widget, interactorsWidget)
    {
        d->layerListToolBox->addWidget(widget);
        widget->show();
    }
    emit selectionChanged();
}
