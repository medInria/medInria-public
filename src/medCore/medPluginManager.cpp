/* medPluginManager.cpp ---
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

#include <QtCore>

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

void medPluginManager::uninitialize()
{
    //do nothing, setting the path only brings about problems when the dtkSettingsEditor is used.
}

void medPluginManager::readSettings(void)
{
    QSettings settings;
    // qSettings should use what is defined in the application (organization and appName)

    settings.beginGroup("plugins");
    QDir plugins_dir = qApp->applicationDirPath() + "/../plugins";

    if (!settings.contains("path"))
    {
        qDebug()<<"Fill in empty path in settings with default path:"
                  << plugins_dir.absolutePath();
        settings.setValue("path", plugins_dir.absolutePath());
    }
    setPath (settings.value("path", plugins_dir.absolutePath()).toString());
    settings.endGroup();

    if(path().isEmpty()) {
        qWarning() << "Your config does not seem to be set correctly.";
        qWarning() << "Please set plugins.path.";
        qWarning() << "Default directory would be: " << plugins_dir.absolutePath();
    }
}

void medPluginManager::writeSettings(void)
{

    QSettings settings;
    settings.beginGroup("plugins");
    settings.setValue("path", path());
    settings.endGroup();
}


QStringList medPluginManager::handlers(const QString& category)
{
    if (d->handlers.contains(category))
        return d->handlers.value(category);

    return QStringList();
}

void medPluginManager::onPluginLoaded(const QString& name)
{
    dtkPlugin *plug = plugin(name);

    QStringList categories;

    if (plug->hasMetaData("category"))
        categories = plug->metaDataValues("category");

    foreach(QString category, categories)
        d->handlers[category] << plug->types();
}

medPluginManager::medPluginManager(void) : dtkPluginManager(), d(new medPluginManagerPrivate)
{
    connect(this, SIGNAL(loaded(const QString&)), this, SLOT(onPluginLoaded(const QString&)));
}

medPluginManager::~medPluginManager(void)
{
    delete d;

    d = NULL;
}

medPluginManager *medPluginManager::s_instance = NULL;
