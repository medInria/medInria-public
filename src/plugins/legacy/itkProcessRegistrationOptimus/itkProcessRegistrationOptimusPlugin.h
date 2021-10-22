#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medPluginLegacy.h>
#include "itkProcessRegistrationOptimusPluginExport.h"

class ITKPROCESSREGISTRATIONOPTIMUSPLUGIN_EXPORT itkProcessRegistrationOptimusPlugin : public medPluginLegacy
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inria.itkProcessRegistrationOptimusPlugin" FILE "itkProcessRegistrationOptimusPlugin.json")
    Q_INTERFACES(dtkPlugin)

public:
     itkProcessRegistrationOptimusPlugin(QObject *parent = nullptr);

     bool initialize() override;

     QString name() const override;
     QString description() const override;
     QStringList authors() const override;
     QStringList contributors() const override;
     QStringList types() const override;
     QString version() const override;
};
