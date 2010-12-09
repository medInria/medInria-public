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

#include "medViewerConfiguration.h"

#include "medToolBox.h"
#include "medViewerToolBoxLayout.h"
#include "medViewContainer.h"
#include "medViewContainerCustom.h"

class medViewerConfigurationPrivate
{
public:
    QWidget *parent;
    medViewerToolBoxLayout *layoutToolBox;
    QList<medToolBox*> toolboxes;
    int viewLayoutType;
    int layoutType;
    int customLayoutType;
    bool databaseVisibility;
    bool layoutToolBoxVisibility;
};

medViewerConfiguration::medViewerConfiguration(QWidget *parent) : QObject(), d(new medViewerConfigurationPrivate)
{
    d->parent = parent;
    d->layoutType = medViewerConfiguration::LeftDbRightTb;
    d->viewLayoutType = medViewContainer::Single;
    d->customLayoutType = 0;
    d->databaseVisibility = true;
    d->layoutToolBoxVisibility = true;
    
    d->layoutToolBox = new medViewerToolBoxLayout(parent);
    connect (d->layoutToolBox, SIGNAL(modeChanged(int)),   this, SIGNAL(layoutModeChanged(int)));
    connect (d->layoutToolBox, SIGNAL(presetClicked(int)), this, SIGNAL(layoutPresetClicked(int)));
    connect (d->layoutToolBox, SIGNAL(split(int,int)),     this, SIGNAL(layoutSplit(int,int)));    
    this->addToolBox(d->layoutToolBox);
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

int medViewerConfiguration::layoutType(void) const
{
    return d->layoutType;
}

void medViewerConfiguration::setViewLayoutType(int type)
{
    d->viewLayoutType = type;
}

int medViewerConfiguration::viewLayoutType(void) const
{
    return d->viewLayoutType;
}

void medViewerConfiguration::setCustomLayoutType(int type)
{
    d->customLayoutType = type;
}

int medViewerConfiguration::customLayoutType(void) const
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

void medViewerConfiguration::setLayoutToolBoxVisibility(bool visibility)
{
    d->layoutToolBoxVisibility = visibility;
}

void medViewerConfiguration::hideLayoutToolBox(void)
{
    d->layoutToolBox->hide();
}

void medViewerConfiguration::showLayoutToolBox(void)
{
    d->layoutToolBox->show();
}

bool medViewerConfiguration::isLayoutToolBoxVisible(void) const
{
    return d->layoutToolBoxVisibility;
}

bool medViewerConfiguration::isLayoutToolBox(const medToolBox * toolbox)
{
    return (toolbox == d->layoutToolBox);
}

void medViewerConfiguration::setupViewContainerStack(medViewContainerStack *container)
{
    Q_UNUSED(container);
}
