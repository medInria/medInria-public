/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medPluginManager.h"

#include <QtCore>

#include <dtkCore/dtkPluginManager.h>
#include <dtkCore/dtkPlugin.h>


class medPluginManagerPrivate
{
public:
    QHash<QString, QStringList> handlers;
    QStringList loadErrors;
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
    QDir plugins_dir;
    QString defaultPath;
#ifdef Q_WS_MAC
    plugins_dir = qApp->applicationDirPath() + "/../PlugIns";
#else
    plugins_dir = qApp->applicationDirPath() + "/../plugins";
#endif
    defaultPath = plugins_dir.absolutePath();

    const char PLUGIN_PATH_VAR_NAME[] = "MEDINRIA_PLUGIN_PATH";
    QByteArray pluginVarArray = qgetenv(PLUGIN_PATH_VAR_NAME);

    if ( !pluginVarArray.isEmpty() )
    {
        setPath( QString(pluginVarArray.constData()));
    }
    else
    {
        setPath(defaultPath);
    }

    if(path().isEmpty()) {
        qWarning() << "Your config does not seem to be set correctly.";
        qWarning() << "Please set plugins.path.";
        qWarning() << "Default directory would be: " << defaultPath;
    }
}



QStringList medPluginManager::handlers(const QString& category)
{
    if (d->handlers.contains(category))
        return d->handlers.value(category);

    return QStringList();
}

void medPluginManager::onPluginLoaded(const QString& name)
{
    qDebug() << " Loading plugin : " << name;

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
    connect(this, SIGNAL(loadError(QString)), this, SLOT(onLoadError(QString)));
}

medPluginManager::~medPluginManager(void)
{
    delete d;

    d = NULL;
}

medPluginManager *medPluginManager::s_instance = NULL;


void medPluginManager::onLoadError(const QString &errorMessage)
{
//    qDebug() << "add error message to pluginManager:";
//    qDebug() << "\t" << errorMessage;
    d->loadErrors << errorMessage;
}

QStringList medPluginManager::loadErrors()
{
    return d->loadErrors;
}
