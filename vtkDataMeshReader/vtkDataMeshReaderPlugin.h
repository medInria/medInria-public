#pragma once

#include <dtkCore/dtkPlugin.h>

#include "vtkDataMeshReaderPluginExport.h"

class VTKDATAMESHREADERPLUGIN_EXPORT vtkDataMeshReaderPluginPrivate;

class VTKDATAMESHREADERPLUGIN_EXPORT vtkDataMeshReaderPlugin : public dtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)
public:
     vtkDataMeshReaderPlugin(QObject *parent = 0);
    ~vtkDataMeshReaderPlugin(void);

    virtual bool initialize(void);
    virtual bool uninitialize(void);

    virtual QString name(void) const;
    virtual QString description(void) const;
    virtual QString version(void) const;

    virtual QStringList tags(void) const;
    virtual QStringList types(void) const;

private:
     vtkDataMeshReaderPluginPrivate *d;
};


