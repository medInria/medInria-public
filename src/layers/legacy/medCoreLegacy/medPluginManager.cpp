/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medPluginManager.h>

#include <dtkCoreSupport/dtkPlugin.h>


class medPluginManagerPrivate
{
public:
    QHash<QString, QStringList> handlers;
    QStringList loadErrors;
};

/**
 * @brief Gets an instance of the Plugin Manager.
 *
 *
 * @param void
 * @return medPluginManager * a pointer to an instance of the singleton.
*/
medPluginManager *medPluginManager::instance(void)
{
    if(!s_instance)
        s_instance = new medPluginManager;
    return s_instance;
}

void medPluginManager::initialize()
{
    dtkPluginManager::initialize();

    emit allPluginsLoaded();
}

/**
 * @brief Uninitialize the manager.
 * @warning does nothing here, writing the path brought problems with the use of the
 * dtkSettingsEditor, that does not set the path in the manager.
 *
 * @param void
*/
void medPluginManager::uninitialize()
{
    //do nothing, setting the path only brings about problems when the dtkSettingsEditor is used.
}

/**
 * @brief Gets the path to the plugins, from the settings.
 * Overrides the method in the parent to use the application name defined
 * in the main function, and do not use dtk as an application name, allowing different
 * applications to use different settings.
 *
 * @param void
*/
void medPluginManager::readSettings(void)
{
    QDir plugins_dir;
    QString defaultPath;
#ifdef Q_OS_MAC
    plugins_dir = qApp->applicationDirPath() + "/../PlugIns";
#elif defined(Q_OS_WIN)
    plugins_dir = qApp->applicationDirPath() + "/plugins_legacy";
#else
    plugins_dir = qApp->applicationDirPath() + "/plugins_legacy";
#endif
    defaultPath = plugins_dir.absolutePath();

    const char PLUGIN_PATH_VAR_NAME[] = "MEDINRIA_PLUGINS_DIR_LEGACY";
    QByteArray pluginVarArray = qgetenv(PLUGIN_PATH_VAR_NAME);

    if ( !pluginVarArray.isEmpty() )
    {
        setPath( QString(pluginVarArray.constData()));
    }
    else
    {
        setPath(defaultPath);
    }

    if(!QDir(path()).exists())
    {
        qWarning() << "Your config does not seem to be set correctly.";
        qWarning() << "Please set "                  << QString(PLUGIN_PATH_VAR_NAME);
        qWarning() << "Default directory would be: " << defaultPath;
        qWarning() << "Actual directory is: "        << path();
    }
}

/**
 * @brief Gets a list of plugins belonging to 'category'
 *
 * @param category The category to use as a filter
 * @return QStringList list of plugin names
*/
QStringList medPluginManager::handlers(const QString& category)
{
    if (d->handlers.contains(category))
        return d->handlers.value(category);

    return QStringList();
}

/**
 * @brief Adds the plugin to the handlers.
 *
 * @param name Name of the loaded plugin
*/
void medPluginManager::onPluginLoaded(const QString& name)
{
    dtkDebug() << " Loading plugin : " << name;

    dtkPlugin *plug = plugin(name);

    QStringList categories;

    if (plug->hasMetaData("category"))
        categories = plug->metaDataValues("category");

    foreach(QString category, categories)
        d->handlers[category] << plug->types();
}

/**
 * @brief Hidden constructor because of the singleton.
 *
 * Use instance() instead.
 * @param void
*/
medPluginManager::medPluginManager(void) : dtkPluginManager(), d(new medPluginManagerPrivate)
{
    connect(this, SIGNAL(loaded(const QString&)), this, SLOT(onPluginLoaded(const QString&)));
    connect(this, SIGNAL(loadError(QString)), this, SLOT(onLoadError(QString)));
}

/**
 * @brief Hidden destructor because of the singleton.
 *
 * @param void
*/
medPluginManager::~medPluginManager(void)
{
    delete d;
    d = NULL;
}

medPluginManager *medPluginManager::s_instance = NULL;


void medPluginManager::onLoadError(const QString &errorMessage)
{
    dtkDebug() << "add error message to pluginManager:";
    dtkDebug() << "\t" << errorMessage;
    d->loadErrors << errorMessage;
}

QStringList medPluginManager::loadErrors()
{
    return d->loadErrors;
}
