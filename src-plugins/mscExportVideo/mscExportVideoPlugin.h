#pragma once

#include <medPlugin.h>
#include <mscExportVideoPluginExport.h>

namespace msc
{

class EXPORTVIDEOPLUGIN_EXPORT ExportVideoPlugin : public medPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
    ExportVideoPlugin(QObject *parent = 0);
    virtual bool initialize();

    virtual QString name() const;
    virtual QString description() const;
    virtual QString version() const;
    virtual QStringList types() const;
};
}
