#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "manualRegistrationPluginExport.h"

#include <medPluginLegacy.h>

class MANUALREGISTRATIONPLUGIN_EXPORT manualRegistrationPlugin : public medPluginLegacy
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inria.manualRegistrationPlugin" FILE "manualRegistrationPlugin.json")
    Q_INTERFACES(dtkPlugin)

public:
    manualRegistrationPlugin(QObject *parent = nullptr);
    virtual bool initialize();

    virtual QString description() const;
    virtual QString name() const;
    virtual QString version() const;
    virtual QStringList types() const;
};
