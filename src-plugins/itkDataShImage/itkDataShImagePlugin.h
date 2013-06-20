// /////////////////////////////////////////////////////////////////
// Generated by dtkPluginGenerator
// /////////////////////////////////////////////////////////////////

#ifndef ITKDATASHIMAGEPLUGIN_H
#define ITKDATASHIMAGEPLUGIN_H

#include <dtkCore/dtkPlugin.h>

#include "itkDataShImagePluginExport.h"

class ITKDATASHIMAGEPLUGIN_EXPORT itkDataShImagePluginPrivate;

class ITKDATASHIMAGEPLUGIN_EXPORT itkDataShImagePlugin : public dtkPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
     itkDataShImagePlugin(QObject *parent = 0);
    ~itkDataShImagePlugin(void);

    virtual bool initialize(void);
    virtual bool uninitialize(void);

    virtual QString name(void) const;
    virtual QString description(void) const;

    virtual QStringList tags(void) const;
    virtual QStringList types(void) const;

private:
     itkDataShImagePluginPrivate *d;
};

#endif
