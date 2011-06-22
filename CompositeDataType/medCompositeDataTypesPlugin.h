#ifndef MEDCOMPOSITEDATATYPESPLUGIN_H
#define MEDCOMPOSITEDATATYPES_H

#include <dtkCore/dtkPlugin.h>

#include "medCompositeDataTypesPluginExport.h"

class MEDCOMPOSITEDATATYPESPLUGIN_EXPORT medCompositeDataTypesPluginPrivate;

class MEDCOMPOSITEDATATYPESPLUGIN_EXPORT medCompositeDataTypesPlugin : public dtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
     medCompositeDataTypesPlugin(QObject *parent = 0);
    ~medCompositeDataTypesPlugin(void);

    virtual bool initialize(void);
    virtual bool uninitialize(void);

    virtual QString name(void) const;
    virtual QString description(void) const;

    virtual QStringList tags(void) const;
    virtual QStringList types(void) const;

private:
     CompositeDataTypePluginPrivate *d;
};

#endif
