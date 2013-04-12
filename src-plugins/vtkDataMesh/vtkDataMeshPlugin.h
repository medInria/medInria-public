#pragma once

#include <dtkCore/dtkPlugin.h>

#include "vtkDataMeshPluginExport.h"

class VTKDATAMESHPLUGIN_EXPORT vtkDataMeshPluginPrivate;

class VTKDATAMESHPLUGIN_EXPORT vtkDataMeshPlugin : public dtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
     vtkDataMeshPlugin(QObject *parent = 0);
    ~vtkDataMeshPlugin();

    virtual bool initialize();
    virtual bool uninitialize();

    virtual QString name() const;
    virtual QString description() const;
    virtual QString version() const;

    virtual QStringList tags() const;
    virtual QStringList types() const;

private:
     vtkDataMeshPluginPrivate *d;
};


