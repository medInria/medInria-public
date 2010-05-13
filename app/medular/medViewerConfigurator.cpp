/* medViewerConfigurator.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 14:13:57 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Thu May 13 14:19:46 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 5
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewerConfiguration.h"
#include "medViewerConfigurator.h"

class medViewerConfiguratorPrivate
{
public:
    QHash<QString, medViewerConfiguration *> configurations;

    QString current;
};

medViewerConfigurator *medViewerConfigurator::instance(void)
{
    if(!s_instance)
        s_instance = new medViewerConfigurator;

    return s_instance;
}

medViewerConfigurator::medViewerConfigurator(void) : QObject(), d(new medViewerConfiguratorPrivate)
{

}

medViewerConfigurator::~medViewerConfigurator(void)
{
    delete d;

    d = NULL;
}

void medViewerConfigurator::addConfiguration(QString name, medViewerConfiguration *configuration)
{
    d->configurations.insert(name, configuration);
}

void medViewerConfigurator::setConfiguration(QString name)
{
    if(!d->current.isNull())
        d->configurations.value(d->current)->setdw();

    if (d->configurations.keys().contains(name))
        d->configurations.value(name)->setup();

    d->current = name;
}

medViewerConfigurator *medViewerConfigurator::s_instance = NULL;
