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

#include <medViewerConfiguration.h>
#include <medViewerConfigurationFactory.h>

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

    medViewerConfiguration * conf;

    if (!d->configurations.contains(name))
    {
        if (conf = medViewerConfigurationFactory::instance()->createConfiguration(name))
        {
            addConfiguration(name, conf);
        }
        else
        {
            qDebug()<< "Configuration" << name << "couldn't be created";
            return;
        }
    }

    //clean old config
//    if(!d->current.isNull())
//    {
//        d->configurations.value(d->current)->setdw();

    if (d->configurations.keys().contains(name))
    {
        //or emit some signal towards medViewerArea
        d->configurations.value(name)->setup();
    }
    d->current = name;
}

medViewerConfigurator *medViewerConfigurator::s_instance = NULL;
