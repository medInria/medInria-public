#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QtCore>

#include <dtkCoreSupport/dtkPluginManager.h>

#include <medCoreExport.h>

#include <memory>

class medPluginManagerPrivate;

/**
 * @brief Load and unload plugins.
 *
*/
class MEDCORE_EXPORT medPluginManager : public dtkPluginManager
{
    Q_OBJECT

public:
    ~medPluginManager();

    static medPluginManager *instance();

    void readSettings();
    void initialize();
    void uninitialize();
    QStringList handlers(const QString& category);

    QStringList loadErrors();

public slots:
    void onPluginLoaded(const QString& name);

protected slots:
     void onLoadError(const QString& errorMessage);

signals:
     void allPluginsLoaded();

private:
    medPluginManager();
    
    static std::shared_ptr<medPluginManager> s_instance;

private:
    medPluginManagerPrivate *d;
};
