#ifndef ITKFILTERSPLUGIN_H
#define ITKFILTERSPLUGIN_H

#include <dtkCore/dtkPlugin.h>

#include "itkFiltersPluginExport.h"

class ITKFILTERSPLUGIN_EXPORT itkFiltersPluginPrivate;

class ITKFILTERSPLUGIN_EXPORT itkFiltersPlugin : public dtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
    itkFiltersPlugin(QObject *parent = 0);
    ~itkFiltersPlugin(void);

    virtual bool initialize(void);
    virtual bool uninitialize(void);

    virtual QString contact(void) const;
    virtual QString description(void) const;
    virtual QString name(void) const;
    virtual QString version(void) const;
    virtual QStringList authors(void) const;
    virtual QStringList contributors(void) const;
    virtual QStringList dependencies(void) const;

    virtual QStringList tags(void) const;
    virtual QStringList types(void) const;

private:
    itkFiltersPluginPrivate *d;
};

#endif // ITKFILTERSPLUGIN_H
