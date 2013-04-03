#pragma once

#include <dtkCore/dtkPlugin.h>

#include "itkFiltersPluginExport.h"

class ITKFILTERSPLUGIN_EXPORT itkFiltersPluginPrivate;

class ITKFILTERSPLUGIN_EXPORT itkFiltersPlugin : public dtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
    itkFiltersPlugin(QObject *parent = 0);
    ~itkFiltersPlugin();

    virtual bool initialize();
    virtual bool uninitialize();

    virtual QString contact() const;
    virtual QString description() const;
    virtual QString name() const;
    virtual QString version() const;
    virtual QStringList authors() const;
    virtual QStringList contributors() const;
    virtual QStringList dependencies() const;

    virtual QStringList tags() const;
    virtual QStringList types() const;

private:
    itkFiltersPluginPrivate *d;
};


