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

#include <medTtkTensorImageExport.h>

class medTtkTensorImagePlugin_EXPORT medTtkTensorImagePluginPrivate;

class medTtkTensorImagePlugin_EXPORT medTtkTensorImagePlugin : public dtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
     medTtkTensorImagePlugin(QObject *parent = 0);
    ~medTtkTensorImagePlugin();

    virtual bool initialize();
    virtual bool uninitialize();

    virtual QString name() const;
    virtual QString description() const;
    virtual QString version() const;

    virtual QStringList tags() const;
    virtual QStringList types() const;

private:
     medTtkTensorImagePluginPrivate *d;
};


