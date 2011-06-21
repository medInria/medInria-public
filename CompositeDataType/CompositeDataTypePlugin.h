#ifndef COMPOSITEDATATYPEPLUGIN_H
#define COMPOSITEDATATYPE_H

#include <dtkCore/dtkPlugin.h>

#include "CompositeDataTypePluginExport.h"

class COMPOSITEDATATYPEPLUGIN_EXPORT CompositeDataTypePluginPrivate;

class COMPOSITEDATATYPEPLUGIN_EXPORT CompositeDataTypePlugin : public dtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
     CompositeDataTypePlugin(QObject *parent = 0);
    ~CompositeDataTypePlugin(void);

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
