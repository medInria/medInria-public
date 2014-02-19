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

class medAbstractWorkspacePrivate
{
public:
    QWidget *parent;

    bool databaseVisibility;
    bool toolBoxesVisibility;

    medTabbedViewContainers * viewContainerStack;
    QHash <QUuid, medViewContainer*> containers;
    medViewContainer* currentContainer;

    QList<medToolBox*> workspaceToolBoxes;
    medToolBox *containerToolBox;
};

medAbstractWorkspace::medAbstractWorkspace(QWidget *parent) : QObject(parent), d(new medAbstractWorkspacePrivate)
{
    d->parent = parent;

    d->containerToolBox = NULL;

    d->viewContainerStack = new medTabbedViewContainers(parent);
    connect(d->viewContainerStack, SIGNAL(newContainer(QUuid&)), this, SLOT(addNewContainer(QUuid&)));
    connect(medViewContainerManager::instance(), SIGNAL(containerAboutToBeDestroyed(QUuid&)), this, SLOT(removeContainer(QUuid&)));
    d->currentContainer = NULL;

    d->databaseVisibility = true;
    d->toolBoxesVisibility = true;
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

medToolBox* medAbstractWorkspace::containerToolBox() const
{
    return d->containerToolBox;
}

void medAbstractWorkspace::setCurrentViewContainer(QUuid& uuid)
{
    d->currentContainer = d->containers.value(uuid);

    qDebug() <<"setCurrentViewContainer : " << d->currentContainer->uuid();
    connect(d->currentContainer, SIGNAL(viewChanged()), this, SIGNAL(currentViewChanged()));
    d->containerToolBox = d->currentContainer->toolBox();

    emit currentContainerChanged();
}

void medAbstractWorkspace::setDatabaseVisibility(bool visibility)
{
    d->databaseVisibility = visibility;
}

bool medAbstractWorkspace::isDatabaseVisible(void) const
{
    return d->databaseVisibility;
}

medViewContainer* medAbstractWorkspace::currentViewContainer() const
{
    return d->currentContainer;
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

void medAbstractWorkspace::addNewContainer(QUuid &uuid)
{
    medViewContainer* newContainer = medViewContainerManager::instance()->container(uuid);
    if(d->containers.contains(uuid))
        return;

    d->containers.insert(uuid, newContainer);
    connect(newContainer, SIGNAL(selected(QUuid&)), this, SLOT(setCurrentViewContainer(QUuid&)));
}


void medAbstractWorkspace::removeContainer(QUuid &uuid)
{
    d->containers.remove(uuid);
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
