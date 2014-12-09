/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkPlugin.h>

#include <medQtDcmDataSourceExport.h>

class medQtDcmDataSourcePLUGIN_EXPORT medQtDcmDataSourcePluginPrivate;

class medQtDcmDataSourcePLUGIN_EXPORT medQtDcmDataSourcePlugin : public dtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
     medQtDcmDataSourcePlugin(QObject *parent = 0);
    ~medQtDcmDataSourcePlugin();

    virtual bool initialize();
    virtual bool uninitialize();

    virtual QString contact() const;
    virtual QString description() const;
    virtual QString name() const;
    virtual QString version() const;
    virtual QStringList authors() const;
    virtual QStringList contributors() const;
    virtual QStringList dependencies() const;

    virtual QStringList tags() const;
    virtual QStringList types() const;

private:
     medQtDcmDataSourcePluginPrivate *d;
};


