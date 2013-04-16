/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medWorkspace.h"

#include <medDataIndex.h>
#include <medSettingsManager.h>

#include "medToolBox.h"
#include "medViewContainer.h"
#include "medCustomViewContainer.h"
#include "medSingleViewContainer.h"
#include "medMultiViewContainer.h"
#include "medTabbedViewContainers.h"


class medWorkspacePrivate
{
public:
    QWidget *parent;
    QList<medToolBox*> toolboxes;
    medWorkspace::LayoutType layoutType;
    int customLayoutType;
    bool databaseVisibility;
    bool toolBoxesVisibility;
    medTabbedViewContainers * viewContainerStack;

};

medWorkspace::medWorkspace(QWidget *parent) : QObject(parent), d(new medWorkspacePrivate)
{
    d->parent = parent;

    d->viewContainerStack = new medTabbedViewContainers(parent);
    connect(d->viewContainerStack,SIGNAL(addTabButtonClicked()),this,SLOT(onAddTabClicked()));
    connect(d->viewContainerStack,SIGNAL(currentChanged(const QString &)),this,SLOT(onContainerChanged(const QString &)));

    d->layoutType = medWorkspace::LeftDbRightTb;
    d->customLayoutType = 0;
    d->databaseVisibility = true;
    d->toolBoxesVisibility = true;
}

medWorkspace::~medWorkspace(void)
{
    delete d;
    d = NULL;
}

void medWorkspace::addToolBox(medToolBox *toolbox)
{
    if (!d->toolboxes.contains(toolbox)) {
        connect (toolbox, SIGNAL(addToolBox(medToolBox*)),    this, SIGNAL(toolboxAdded  (medToolBox*)));
        connect (toolbox, SIGNAL(removeToolBox(medToolBox*)), this, SIGNAL(toolboxRemoved(medToolBox*)));
        d->toolboxes.append(toolbox);
        emit toolboxAdded(toolbox);
    }
}

void medWorkspace::removeToolBox(medToolBox *toolbox)
{
    if (d->toolboxes.contains(toolbox)) {
        disconnect (toolbox, SIGNAL(addToolBox(medToolBox*)),    this, SIGNAL(toolboxAdded  (medToolBox*)));
        disconnect (toolbox, SIGNAL(removeToolBox(medToolBox*)), this, SIGNAL(toolboxRemoved(medToolBox*)));
        d->toolboxes.removeOne(toolbox);
        emit toolboxRemoved(toolbox);
    }
}

QList<medToolBox*> medWorkspace::toolBoxes(void) const
{
    return d->toolboxes;
}

void medWorkspace::setLayoutType(medWorkspace::LayoutType type)
{
    d->layoutType = type;
}

medWorkspace::LayoutType medWorkspace::layoutType(void) const
{
    return d->layoutType;
}

void medWorkspace::setCurrentViewContainer(const QString& name)
{
    d->viewContainerStack->setContainer(name);
}

void medWorkspace::onContainerChanged(const QString &name)
{
    if (!d->viewContainerStack->container(name))
        return;

    QString containerType = d->viewContainerStack->container(name)->identifier();
    emit setLayoutTab(containerType);
}

void medWorkspace::setCustomPreset(int type)
{
    d->customLayoutType = type;
}

int medWorkspace::customLayoutPreset(void) const
{
    return d->customLayoutType;
}

void medWorkspace::setDatabaseVisibility(bool visibility)
{
    d->databaseVisibility = visibility;
}

bool medWorkspace::isDatabaseVisible(void) const
{
    return d->databaseVisibility;
}

medViewContainer* medWorkspace::currentViewContainer() const
{
    return d->viewContainerStack->current();
}

QString medWorkspace::currentViewContainerName() const
{
    return d->viewContainerStack->currentName();
}

medTabbedViewContainers* medWorkspace::stackedViewContainers() const
{
    return d->viewContainerStack;
}

QString medWorkspace::addDefaultTypeContainer(const QString& name)
{
    //Default container:
    //get default Layout type from settings:
    medSettingsManager * mnger = medSettingsManager::instance();
    QString layout = mnger->value("startup","default_container_layout",
                                       "Multi").toString();
    
    QString createdName = name;
    if (layout == "Custom")
    {
        if (name.isEmpty())
            createdName = addCustomContainer();
        else
            createdName = addCustomContainer(name);
    } else if (layout == "Single")
    {
        if(name.isEmpty())
            createdName = addSingleContainer();
        else
            createdName = addSingleContainer(name);
    }
    else
    {
        if(name.isEmpty())
            createdName = addMultiContainer();
        else
            createdName = addMultiContainer(name);
    }
    
    return createdName;
}

QString medWorkspace::addSingleContainer(const QString& name)
{
    if (!this->stackedViewContainers()->container(name))
    {
        this->stackedViewContainers()->addContainer (name, new medSingleViewContainer());
        return name;
    }
    else
    {
        unsigned int i = 1;
        QString newName = name + " ";
        newName += QString::number(i);
        while (this->stackedViewContainers()->container(newName))
        {
            ++i;
            newName = name + " ";
            newName += QString::number(i);
        }
        
        this->stackedViewContainers()->addContainer (newName, new medSingleViewContainer());
        return newName;
    }
}

QString medWorkspace::addMultiContainer(const QString& name)
{
    if (!this->stackedViewContainers()->container(name))
    {
        this->stackedViewContainers()->addContainer (name, new medMultiViewContainer());
        return name;
    }
    else
    {
        unsigned int i = 1;
        QString newName = name + " ";
        newName += QString::number(i);
        while (this->stackedViewContainers()->container(newName))
        {
            ++i;
            newName = name + " ";
            newName += QString::number(i);
        }

        this->stackedViewContainers()->addContainer (newName, new medMultiViewContainer());
        return newName;
    }
}

QString medWorkspace::addCustomContainer(const QString& name)
{
    if (!this->stackedViewContainers()->container(name))
    {
        this->stackedViewContainers()->addContainer (name, new medCustomViewContainer());
        return name;
    }
    else
    {
        unsigned int i = 1;
        QString newName = name + " ";
        newName += QString::number(i);
        while (this->stackedViewContainers()->container(newName))
        {
            ++i;
            newName = name + " ";
            newName += QString::number(i);
        }
        
        this->stackedViewContainers()->addContainer (newName, new medCustomViewContainer());
        return newName;
    }
}


//void medWorkspace::patientChanged(const medDataIndex& id)
//{
//    Q_UNUSED(id);
//}

void medWorkspace::clear()
{
    //medViewContainer* container;
    QList<QString> names = d->viewContainerStack->keys();
    foreach(QString name, names)
    {
        d->viewContainerStack->removeContainer(name);
    }
    setupViewContainerStack();
    clearToolBoxes();
    return;
}

void medWorkspace::onButtonChecked( const QString & buttonGroup )
{
    medToolBox * sender = qobject_cast< medToolBox * >( this->sender() );
    foreach ( medToolBox * toolbox, d->toolboxes )
        if ( toolbox != sender )
            toolbox->uncheckButtons( buttonGroup );
}

void medWorkspace::setToolBoxesVisibility (bool value)
{
    d->toolBoxesVisibility = value;
}

bool medWorkspace::areToolBoxesVisible() const
{
    return d->toolBoxesVisibility;
}


void medWorkspace::clearToolBoxes()
{
    foreach(medToolBox* tb,d->toolboxes)
    {
        tb->clear();
    }
}

void medWorkspace::onAddTabClicked()
{
    QString name = this->description();

    QString realName = this->addDefaultTypeContainer(name);
    this->stackedViewContainers()->setContainer(realName);
}
