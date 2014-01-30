/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkPlugin.h>

#include <ITKProcessExamplePluginExport.h>

class ITKPROCESSEXAMPLEPLUGIN_EXPORT ITKProcessExamplePluginPrivate;

class ITKPROCESSEXAMPLEPLUGIN_EXPORT ITKProcessExamplePlugin : public dtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
     ITKProcessExamplePlugin(QObject *parent = 0);
    ~ITKProcessExamplePlugin();

    virtual bool initialize();
    virtual bool uninitialize();

    virtual QString contact() const;
    virtual QString description() const;
    virtual QString name() const;
    virtual QString version() const;

    virtual QStringList tags() const;
    virtual QStringList types() const;

private:
    ITKProcessExamplePluginPrivate *d;
};


