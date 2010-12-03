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

class medViewerConfigurationPrivate
{
public:
    QWidget *parent;
    medViewerToolBoxLayout *layoutToolBox;
    QList<medToolBox*> toolboxes;
    int toolBoxesLocation;
    int layoutType;
    int customLayoutType;
    int databaseLocation;
    int databaseVisibility;
};

medViewerConfiguration::medViewerConfiguration(QWidget *parent) : QObject(), d(new medViewerConfigurationPrivate)
{
    d->parent = parent;
    d->toolBoxesLocation = medRight;
    d->layoutType = medLayoutSingle;
    d->customLayoutType = 0;
    d->databaseLocation = medLeft;
    d->databaseVisibility = 1;
    
    d->layoutToolBox = new medViewerToolBoxLayout(parent);
    this->addToolBox(d->layoutToolBox);
}

medViewerConfiguration::~medViewerConfiguration(void)
{
    delete d;
    d = NULL;
}

void medViewerConfiguration::addToolBox(medToolBox *toolbox)
{
    if (!d->toolboxes.contains(toolbox))
        d->toolboxes.append(toolbox);
}

QList<medToolBox*> medViewerConfiguration::toolBoxes(void) const
{
    return d->toolboxes;
}

void medViewerConfiguration::setToolBoxesLocation(medPosition pos)
{
    d->toolBoxesLocation = pos;
}

int medViewerConfiguration::toolBoxesLocation(void) const
{
    return d->toolBoxesLocation;
}

void medViewerConfiguration::setLayoutType(medLayoutType type)
{
    d->layoutType = type;
}

int medViewerConfiguration::layoutType(void) const
{
    return d->layoutType;
}

void medViewerConfiguration::setCustomLayoutType(int type)
{
    d->customLayoutType = type;
}

int medViewerConfiguration::customLayoutType(void) const
{
    return d->customLayoutType;
}

void medViewerConfiguration::setDatabaseLocation(medPosition pos)
{
    d->databaseLocation = pos;
}

int medViewerConfiguration::databaseLocation(void) const
{
    return d->databaseLocation;
}

void medViewerConfiguration::hideDatabase(void)
{
    d->databaseVisibility = 0;
}

void medViewerConfiguration::showDatabase(void)
{
    d->databaseVisibility = 1;
}

int medViewerConfiguration::databaseVisibility(void) const
{
    return d->databaseVisibility;
}
