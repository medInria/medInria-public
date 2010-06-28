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

#include "medViewerArea.h"
#include "medViewerConfiguration.h"

#include <medGui/medToolBox.h>

class medViewerConfigurationPrivate
{
public:
    medViewerArea *parent;

    int container_index;

    QHash<medToolBox *, bool> toolboxes;
};

medViewerConfiguration::medViewerConfiguration(medViewerArea *parent) : QObject(), d(new medViewerConfigurationPrivate)
{
    d->parent = parent;
}

medViewerConfiguration::~medViewerConfiguration(void)
{
    delete d;

    d = NULL;
}

void medViewerConfiguration::setup(void)
{
    foreach(medToolBox *toolbox, d->toolboxes.keys())
        toolbox->setVisible(d->toolboxes.value(toolbox));

    d->parent->switchToContainer(d->container_index);
}

void medViewerConfiguration::setdw(void)
{
    foreach(medToolBox *toolbox, d->toolboxes.keys())
        toolbox->setVisible(!(d->toolboxes.value(toolbox)));

    // TODO: update container index to save current state
}

void medViewerConfiguration::attach(int index)
{
    d->container_index = index;
}

void medViewerConfiguration::attach(medToolBox *toolbox, bool visible)
{
    d->toolboxes.insert(toolbox, visible);
}
