/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medPlugin.h>
#include <itkProcessRegistrationDiffeomorphicDemonsPluginExport.h>

class ITKPROCESSREGISTRATIONDIFFEOMORPHICDEMONSPLUGIN_EXPORT itkProcessRegistrationDiffeomorphicDemonsPlugin : public medPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
    itkProcessRegistrationDiffeomorphicDemonsPlugin(QObject *parent = 0);
    virtual bool initialize();

    virtual QString description() const;
    virtual QString name() const;
    virtual QString version() const;
    virtual QStringList authors() const;
    virtual QStringList contributors() const;
    virtual QStringList types() const;
};
