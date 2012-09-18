/* medViewerWorkspace.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 14:12:19 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Jun 15 11:04:35 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 7
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */
#include "medViewerWorkspace.h"

#include <medDataIndex.h>
#include <medSettingsManager.h>

#include "medToolBox.h"
#include "medViewContainer.h"
#include "medViewContainerCustom.h"
#include "medViewContainerSingle.h"
#include "medViewContainerMulti.h"
#include "medTabbedViewContainers.h"


class medViewerWorkspacePrivate
{
public:
    QWidget *parent;
    QList<medToolBox*> toolboxes;
    medViewerWorkspace::LayoutType layoutType;
    int customLayoutType;
    bool databaseVisibility;
    bool toolBoxesVisibility;
    medTabbedViewContainers * viewContainerStack;

};

medViewerWorkspace::medViewerWorkspace(QWidget *parent) : QObject(parent), d(new medViewerWorkspacePrivate)
{
    d->parent = parent;

    d->viewContainerStack = new medTabbedViewContainers(parent);
    connect(d->viewContainerStack,SIGNAL(addTabButtonClicked()),this,SLOT(onAddTabClicked()));
    connect(d->viewContainerStack,SIGNAL(currentChanged(const QString &)),this,SLOT(onContainerChanged(const QString &)));

    d->layoutType = medViewerWorkspace::LeftDbRightTb;
    d->customLayoutType = 0;
    d->databaseVisibility = true;
    d->toolBoxesVisibility = true;
}

medViewerWorkspace::~medViewerWorkspace(void)
{
    delete d;
    d = NULL;
}

void medViewerWorkspace::addToolBox(medToolBox *toolbox)
{
    if (!d->toolboxes.contains(toolbox)) {
        connect (toolbox, SIGNAL(addToolBox(medToolBox*)),    this, SIGNAL(toolboxAdded  (medToolBox*)));
        connect (toolbox, SIGNAL(removeToolBox(medToolBox*)), this, SIGNAL(toolboxRemoved(medToolBox*)));
        d->toolboxes.append(toolbox);
        emit toolboxAdded(toolbox);
    }
}

void medViewerWorkspace::removeToolBox(medToolBox *toolbox)
{
    if (d->toolboxes.contains(toolbox)) {
        disconnect (toolbox, SIGNAL(addToolBox(medToolBox*)),    this, SIGNAL(toolboxAdded  (medToolBox*)));
        disconnect (toolbox, SIGNAL(removeToolBox(medToolBox*)), this, SIGNAL(toolboxRemoved(medToolBox*)));
        d->toolboxes.removeOne(toolbox);
        emit toolboxRemoved(toolbox);
    }
}

QList<medToolBox*> medViewerWorkspace::toolBoxes(void) const
{
    return d->toolboxes;
}

void medViewerWorkspace::setLayoutType(medViewerWorkspace::LayoutType type)
{
    d->layoutType = type;
}

medViewerWorkspace::LayoutType medViewerWorkspace::layoutType(void) const
{
    return d->layoutType;
}

void medViewerWorkspace::setCurrentViewContainer(const QString& name)
{
    d->viewContainerStack->setContainer(name);
}

void medViewerWorkspace::onContainerChanged(const QString &name)
{
    if (!d->viewContainerStack->container(name))
        return;

    QString containerType = d->viewContainerStack->container(name)->identifier();
    emit setLayoutTab(containerType);
}

void medViewerWorkspace::setCustomPreset(int type)
{
    d->customLayoutType = type;
}

int medViewerWorkspace::customLayoutPreset(void) const
{
    return d->customLayoutType;
}

void medViewerWorkspace::setDatabaseVisibility(bool visibility)
{
    d->databaseVisibility = visibility;
}

bool medViewerWorkspace::isDatabaseVisible(void) const
{
    return d->databaseVisibility;
}

medViewContainer* medViewerWorkspace::currentViewContainer() const
{
    return d->viewContainerStack->current();
}

QString medViewerWorkspace::currentViewContainerName() const
{
    return d->viewContainerStack->currentName();
}

medTabbedViewContainers* medViewerWorkspace::stackedViewContainers() const
{
    return d->viewContainerStack;
}

void medViewerWorkspace::addDefaultTypeContainer(const QString& name)
{
    //Default container:
    //get default Layout type from settings:
    medSettingsManager * mnger = medSettingsManager::instance();
    QString layout = mnger->value("startup","default_container_layout",
                                       "Multi").toString();
    if (layout == "Custom")
    {
        if (name.isEmpty())
            addCustomContainer();
        else addCustomContainer(name);
    } else if (layout == "Single")
    {
        if(name.isEmpty())
            addSingleContainer();
        else addSingleContainer(name);
    }
    else
    {
        if(name.isEmpty())
            addMultiContainer();
        else addMultiContainer(name);
    }
}

void medViewerWorkspace::addSingleContainer(const QString& name)
{
    if (!this->stackedViewContainers()->container(name))
        this->stackedViewContainers()->addContainer (name, new medViewContainerSingle());
    else
        qDebug() << "Container" << name << "already exists in this workspaces";
}

QString medViewerWorkspace::addMultiContainer(const QString& name)
{
    if (!this->stackedViewContainers()->container(name))
    {
        this->stackedViewContainers()->addContainer (name, new medViewContainerMulti());
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

        this->stackedViewContainers()->addContainer (newName, new medViewContainerMulti());
        return newName;
    }
}

void medViewerWorkspace::addCustomContainer(const QString& name)
{
    if (!this->stackedViewContainers()->container(name))
        this->stackedViewContainers()->addContainer (name, new medViewContainerCustom());
    else
        qDebug() << "Container" << name << "already exists in this workspace";
}


//void medViewerWorkspace::patientChanged(const medDataIndex& id)
//{
//    Q_UNUSED(id);
//}

void medViewerWorkspace::clear()
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

void medViewerWorkspace::onButtonChecked( const QString & buttonGroup )
{
    medToolBox * sender = qobject_cast< medToolBox * >( this->sender() );
    foreach ( medToolBox * toolbox, d->toolboxes )
        if ( toolbox != sender )
            toolbox->uncheckButtons( buttonGroup );
}

void medViewerWorkspace::setToolBoxesVisibility (bool value)
{
    d->toolBoxesVisibility = value;
}

bool medViewerWorkspace::areToolBoxesVisible (void) const
{
    return d->toolBoxesVisibility;
}


void medViewerWorkspace::clearToolBoxes()
{
    foreach(medToolBox* tb,d->toolboxes)
    {
        tb->clear();
    }
}

void medViewerWorkspace::onAddTabClicked()
{
    QString name = this->description();
    QString realName = name;

    unsigned int suppTag = 0;
    while (this->stackedViewContainers()->container(realName))
    {
        suppTag++;
        realName = name + " ";
        realName += QString::number(suppTag);
    }

    this->addDefaultTypeContainer(realName);
    this->stackedViewContainers()->setContainer(realName);
}
