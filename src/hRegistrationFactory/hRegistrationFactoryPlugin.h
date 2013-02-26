// /////////////////////////////////////////////////////////////////
// Generated by medPluginGenerator
// /////////////////////////////////////////////////////////////////

#ifndef HREGISTRATIONFACTORYPLUGIN_H
#define HREGISTRATIONFACTORYPLUGIN_H

#include <dtkCore/dtkPlugin.h>

#include "hRegistrationFactoryExport.h"

class HREGISTRATIONFACTORY_EXPORT hRegistrationFactoryPluginPrivate;

class HREGISTRATIONFACTORY_EXPORT hRegistrationFactoryPlugin : public dtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)
    
public:
    hRegistrationFactoryPlugin(QObject *parent = 0);
    ~hRegistrationFactoryPlugin(void);
    
    virtual bool initialize(void);
    virtual bool uninitialize(void);
    
    virtual QString name(void) const;
    virtual QString identifier(void) const;
    virtual QString description(void) const;
    virtual QString contact(void) const;
    virtual QString version (void) const;
    virtual QStringList authors(void) const;
    virtual QStringList contributors(void) const;
    virtual QStringList dependencies(void) const;
    
    virtual QStringList tags(void) const;
    virtual QStringList types(void) const;
    
private:
    hRegistrationFactoryPluginPrivate *d;
};

#endif
