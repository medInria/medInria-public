#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkPlugin.h>
#include "itkProcessRegistrationOptimusPluginExport.h"

class ITKPROCESSREGISTRATIONOPTIMUSPLUGIN_EXPORT itkProcessRegistrationOptimusPlugin : public dtkPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inria.itkProcessRegistrationOptimusPlugin" FILE "itkProcessRegistrationOptimusPlugin.json")
    Q_INTERFACES(dtkPlugin)

public:
     itkProcessRegistrationOptimusPlugin(QObject *parent = 0);
    ~itkProcessRegistrationOptimusPlugin(void);

     virtual bool initialize(void);
     virtual bool uninitialize(void);

     virtual QString name(void) const;
     virtual QString description(void) const;
     virtual QStringList authors(void) const;
     virtual QStringList contributors(void) const;
     virtual QString contact()const;
     virtual QStringList tags(void) const;
     virtual QStringList types(void) const;
     virtual QString version() const;
};
