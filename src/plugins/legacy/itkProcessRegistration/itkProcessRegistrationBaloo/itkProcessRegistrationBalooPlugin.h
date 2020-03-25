#pragma once

#include <dtkCoreSupport/dtkPlugin.h>
#include "itkProcessRegistrationBalooPluginExport.h"

class ITKPROCESSREGISTRATIONBALOOPLUGIN_EXPORT itkProcessRegistrationBalooPlugin : public dtkPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inria.itkProcessRegistrationBalooPlugin" FILE "itkProcessRegistrationBalooPlugin.json")
    Q_INTERFACES(dtkPlugin)

public:
     itkProcessRegistrationBalooPlugin(QObject *parent = 0);
    ~itkProcessRegistrationBalooPlugin(void);

     virtual bool initialize(void);
     virtual bool uninitialize(void);

     virtual QString name(void) const;
     virtual QString description(void) const;
     virtual QStringList authors(void) const;
     virtual QStringList contributors(void) const;
     virtual QString contact()const;
     virtual QString version()const;
     virtual QStringList tags(void) const;
     virtual QStringList types(void) const;
};
