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
#include <medItkSHImageExport.h>

class MEDITKSHIMAGEPLUGIN_EXPORT medItkSHImagePluginPrivate;

class MEDITKSHIMAGEPLUGIN_EXPORT medItkSHImagePlugin : public dtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
     medItkSHImagePlugin(QObject *parent = 0);
    ~medItkSHImagePlugin();

    virtual bool initialize();
    virtual bool uninitialize();

    virtual QString name() const;
    virtual QString description() const;

    virtual QStringList tags() const;
    virtual QStringList types() const;

    virtual QString version() const;
    virtual QString contact(void) const;
    virtual QStringList authors() const;
    virtual QStringList contributors() const;

private:
     medItkSHImagePluginPrivate *d;
};
