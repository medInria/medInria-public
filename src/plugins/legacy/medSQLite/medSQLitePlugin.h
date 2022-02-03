#pragma once
/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2020. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include <medPluginLegacy.h>
#include "medSQLitePluginExport.h"

class MEDSQLITEPLUGIN_EXPORT medSQLitePlugin : public medPluginLegacy
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inria.medSQLitePlugin" FILE "medSQLitePlugin.json")
    Q_INTERFACES(dtkPlugin)

public:
    medSQLitePlugin(QObject *parent = nullptr);
    ~medSQLitePlugin();
    virtual bool initialize();
    
    virtual QString name() const;
    virtual QString description() const;
    virtual QString version () const;
    virtual QStringList types() const;
};
