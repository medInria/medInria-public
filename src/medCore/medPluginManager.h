/* medPluginManager.h -
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Oct 28 15:36:13 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Feb 22 21:22:12 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 15
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef MEDPLUGINMANAGER_H
#define MEDPLUGINMANAGER_H

#include <QtCore>

#include <dtkCore/dtkPluginManager.h>

#include "medCoreExport.h"

class medPluginManagerPrivate;

/**
 * @brief Load and unload plugins for medinria.
 *
*/
class MEDCORE_EXPORT medPluginManager : public dtkPluginManager
{
    Q_OBJECT

public:
    /**
     * @brief Gets an instance of the Plugin Manager.
     *
     *
     * @param void
     * @return medPluginManager * a pointer to an instance of the singleton.
    */
    static medPluginManager *instance(void);

    /**
     * @brief Gets the path to the plugins, from the settings.
     * Overrides the method in the parent to use the application name defined
     * in the main function, and do not use dtk as an application name, allowing different
     * applications to use different settings.
     *
     * @param void
    */
    void  readSettings(void);

    /**
     * @brief Uninitialize the manager.
     * @warning does nothing here, writing the path brought problems with the use of the
     * dtkSettingsEditor, that does not set the path in the manager.
     *
     * @param void
    */
    void uninitialize();

    /**
     * @brief Writes the plugin path to the settings.
     *
     *
     * @param void
    */
    void writeSettings(void);

    /**
     * @brief Gets a list of plugins belonging to 'category'
     *
     * @param category The category to use as a filter
     * @return QStringList list of plugin names
    */
    QStringList handlers(const QString& category);

public slots:

    /**
     * @brief Adds the plugin to the handlers.
     *
     * @param name Name of the loaded plugin
    */
    void onPluginLoaded(const QString& name);

protected:
     /**
      * @brief Hidden constructor because of the singleton.
      *
      * Use instance() instead.
      * @param void
     */
     medPluginManager(void);

    /**
     * @brief Hidden destructor because of the singleton.
     *
     * @param void
    */
    ~medPluginManager(void);

private:
    static medPluginManager *s_instance; /**< TODO */

private:
    medPluginManagerPrivate *d; /**< TODO */
};

#endif // MEDPLUGINMANAGER_H
