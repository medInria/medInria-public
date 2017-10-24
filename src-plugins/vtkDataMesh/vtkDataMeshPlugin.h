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
#include <vtkDataMeshPluginExport.h>

class VTKDATAMESHPLUGIN_EXPORT vtkDataMeshPluginPrivate;

class VTKDATAMESHPLUGIN_EXPORT vtkDataMeshPlugin : public medPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
     vtkDataMeshPlugin(QObject *parent = 0);
    ~vtkDataMeshPlugin();

    virtual bool initialize();

    virtual QString name() const;
    virtual QString description() const;
    virtual QString version() const;
    virtual QStringList tags() const;
    virtual QStringList types() const;

private:
     vtkDataMeshPluginPrivate *d;
};
