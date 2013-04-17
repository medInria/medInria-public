/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore>

#include <dtkCore/dtkPluginManager.h>

#include "medCoreExport.h"

class medPluginManagerPrivate;

/**
 * @brief Load and unload plugins for medInria.
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
    static medPluginManager *instance();

    /**
     * @brief Gets the path to the plugins, from the settings.
     * Overrides the method in the parent to use the application name defined
     * in the main function, and do not use dtk as an application name, allowing different
     * applications to use different settings.
     *
     * @param void
    */
    void  readSettings();

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
    void writeSettings();

    /**
     * @brief Gets a list of plugins belonging to 'category'
     *
     * @param category The category to use as a filter
     * @return QStringList list of plugin names
    */
    QStringList handlers(const QString& category);

    QStringList loadErrors();

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
     medPluginManager();


    /**
     * @brief Hidden destructor because of the singleton.
     *
     * @param void
    */
    ~medPluginManager();

protected slots:
     void onLoadError(const QString& errorMessage);

private:
    static medPluginManager *s_instance; /**< TODO */

private:
    medPluginManagerPrivate *d; /**< TODO */
};


