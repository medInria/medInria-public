/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore>

#include <dtkCoreSupport/dtkPluginManager.h>

#include <medCoreExport.h>

class medPluginManagerPrivate;

/**
 * @brief Load and unload plugins for medInria.
 *
*/
class MEDCORE_EXPORT medPluginManager : public dtkPluginManager
{
    Q_OBJECT

public:
    static medPluginManager *instance();

    void readSettings();
    void initialize();
    void uninitialize();
    QStringList handlers(const QString& category);

    QStringList loadErrors();

public slots:
    void onPluginLoaded(const QString& name);

protected:
     medPluginManager();
    ~medPluginManager();

protected slots:
     void onLoadError(const QString& errorMessage);

signals:
     void allPluginsLoaded();

private:
    static medPluginManager *s_instance; /**< TODO */

private:
    medPluginManagerPrivate *d; /**< TODO */
};


