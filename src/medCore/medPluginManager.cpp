/* medPluginManager.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Oct 28 18:09:54 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Feb 22 21:28:17 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 52
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medPluginManager.h"

#include <dtkCore/dtkPluginManager.h>
#include <dtkCore/dtkPlugin.h>


class medPluginManagerPrivate
{
public:
    QHash<QString, QStringList> handlers;
};

medPluginManager *medPluginManager::instance(void)
{
    if(!s_instance)
        s_instance = new medPluginManager;
    return s_instance;
}

void medPluginManager::initialize(void)
{
    dtkPluginManager::instance()->initialize();
}

void medPluginManager::uninitialize(void)
{
    dtkPluginManager::instance()->uninitialize();
}

void medPluginManager::readSettings(void)
{
    dtkPluginManager::instance()->readSettings();
}

void medPluginManager::writeSettings(void)
{
    dtkPluginManager::instance()->writeSettings();
}

QStringList medPluginManager::handlers(const QString& category)
{
    if (d->handlers.contains(category))
        return d->handlers.value(category);

    return QStringList();
}

void medPluginManager::onPluginLoaded(const QString& name)
{
    dtkPlugin *plugin = dtkPluginManager::instance()->plugin(name);

    QStringList categories;

    if (plugin->hasMetaData("category"))
        categories = plugin->metaDataValues("category");

    foreach(QString category, categories)
        d->handlers[category] << plugin->types();
}

medPluginManager::medPluginManager(void) : QObject(), d(new medPluginManagerPrivate)
{
    connect(dtkPluginManager::instance(), SIGNAL(loaded(const QString&)), this, SLOT(onPluginLoaded(const QString&)));
}

medPluginManager::~medPluginManager(void)
{
    delete d;

    d = NULL;
}

medPluginManager *medPluginManager::s_instance = NULL;
