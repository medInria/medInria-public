#ifndef VTKDATAMESHPLUGIN_H
#define VTKDATAMESHPLUGIN_H

#include <dtkCore/dtkPlugin.h>

#include "vtkDataMeshPluginExport.h"

class VTKDATAMESHPLUGIN_EXPORT vtkDataMeshPluginPrivate;

class VTKDATAMESHPLUGIN_EXPORT vtkDataMeshPlugin : public dtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
     vtkDataMeshPlugin(QObject *parent = 0);
    ~vtkDataMeshPlugin(void);

    virtual bool initialize(void);
    virtual bool uninitialize(void);

    virtual QString name(void) const;
    virtual QString description(void) const;

    virtual QStringList tags(void) const;
    virtual QStringList types(void) const;

private:
     vtkDataMeshPluginPrivate *d;
};

#endif
