/* medViewerConfiguration.cpp --- 
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

#include <medCore/medDataIndex.h>

#include "medViewerConfiguration.h"

#include "medToolBox.h"
#include "medViewContainer.h"
#include "medViewContainerCustom.h"
#include "medViewContainerSingle.h"
#include "medViewContainerMulti.h"
#include "medStackedViewContainers.h"

class medViewerConfigurationPrivate
{
public:
    QWidget *parent;
    QList<medToolBox*> toolboxes;
    medViewerConfiguration::LayoutType layoutType;
    int customLayoutType;
    bool databaseVisibility;
    bool toolBoxesVisibility;
    medStackedViewContainers * viewContainerStack;
    
};

medViewerConfiguration::medViewerConfiguration(QWidget *parent) : QObject(), d(new medViewerConfigurationPrivate)
{
    d->parent = parent;
    d->viewContainerStack = new medStackedViewContainers(parent);
    d->layoutType = medViewerConfiguration::LeftDbRightTb;
    d->customLayoutType = 0;
    d->databaseVisibility = true;
    d->toolBoxesVisibility = true;
}

medViewerConfiguration::~medViewerConfiguration(void)
{
    delete d;
    d = NULL;
}

void medViewerConfiguration::addToolBox(medToolBox *toolbox)
{
    if (!d->toolboxes.contains(toolbox)) {
        connect (toolbox, SIGNAL(addToolBox(medToolBox*)),    this, SIGNAL(toolboxAdded  (medToolBox*)));
        connect (toolbox, SIGNAL(removeToolBox(medToolBox*)), this, SIGNAL(toolboxRemoved(medToolBox*)));
        d->toolboxes.append(toolbox);
        emit toolboxAdded(toolbox);
    }
}

void medViewerConfiguration::removeToolBox(medToolBox *toolbox)
{
    if (d->toolboxes.contains(toolbox)) {
        disconnect (toolbox, SIGNAL(addToolBox(medToolBox*)),    this, SIGNAL(toolboxAdded  (medToolBox*)));
        disconnect (toolbox, SIGNAL(removeToolBox(medToolBox*)), this, SIGNAL(toolboxRemoved(medToolBox*)));
        d->toolboxes.removeOne(toolbox);
        emit toolboxRemoved(toolbox);
    }
}

QList<medToolBox*> medViewerConfiguration::toolBoxes(void) const
{
    return d->toolboxes;
}

void medViewerConfiguration::setLayoutType(medViewerConfiguration::LayoutType type)
{
    d->layoutType = type;
}

medViewerConfiguration::LayoutType medViewerConfiguration::layoutType(void) const
{
    return d->layoutType;
}

void medViewerConfiguration::setCurrentViewContainer(const QString& name)
{
    d->viewContainerStack->setContainer(name);
}

void medViewerConfiguration::setCustomPreset(int type)
{
    d->customLayoutType = type;
}

int medViewerConfiguration::customLayoutPreset(void) const
{
    return d->customLayoutType;
}

void medViewerConfiguration::setDatabaseVisibility(bool visibility)
{
    d->databaseVisibility = visibility;
}

bool medViewerConfiguration::isDatabaseVisible(void) const
{
    return d->databaseVisibility;
}

medViewContainer* medViewerConfiguration::currentViewContainer() const
{
    return d->viewContainerStack->current();
}

QString medViewerConfiguration::currentViewContainerName() const
{
    return d->viewContainerStack->currentName();
}

medStackedViewContainers* medViewerConfiguration::stackedViewContainers() const
{
    return d->viewContainerStack;
}

void medViewerConfiguration::addSingleContainer(const QString& name)
{
    if (!this->stackedViewContainers()->container(name))
        this->stackedViewContainers()->addContainer (name, new medViewContainerSingle());
    else
        qDebug() << "Container" << name << "already exists in this configurations";
}

void medViewerConfiguration::addMultiContainer(const QString& name)
{
    if (!this->stackedViewContainers()->container(name))
        this->stackedViewContainers()->addContainer (name, new medViewContainerMulti());
    else
        qDebug() << "Container" << name << "already exists in this configurations";
}

void medViewerConfiguration::addCustomContainer(const QString& name)
{
    if (!this->stackedViewContainers()->container(name))
        this->stackedViewContainers()->addContainer (name, new medViewContainerCustom());
    else
        qDebug() << "Container" << name << "already exists in this configurations";
}


//void medViewerConfiguration::patientChanged(const medDataIndex& id)
//{
//    Q_UNUSED(id);
//}

void medViewerConfiguration::clear()
{
    //medViewContainer* container;
    QList<QString> names = d->viewContainerStack->keys();
    foreach(QString name ,names)
    {
        d->viewContainerStack->removeContainer(name);
    }
    setupViewContainerStack();
    clearToolBoxes();
    return;
}

void medViewerConfiguration::onButtonChecked( const QString & buttonGroup )
{
    medToolBox * sender = dynamic_cast< medToolBox * >( this->sender() );
    foreach ( medToolBox * toolbox, d->toolboxes )
        if ( toolbox != sender )
            toolbox->uncheckButtons( buttonGroup );
}

void medViewerConfiguration::setToolBoxesVisibility (bool value)
{
    d->toolBoxesVisibility = value;
}

bool medViewerConfiguration::areToolBoxesVisible (void) const
{
    return d->toolBoxesVisibility;
}


void medViewerConfiguration::clearToolBoxes()
{
    foreach(medToolBox* tb,d->toolboxes)
    {
        tb->clear();
    }
}
