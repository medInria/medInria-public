#pragma once

#include <dtkCoreSupport/dtkPlugin.h>
#include "itkINRDataImageReaderPluginExport.h"

class ITKINRDATAIMAGEREADERPLUGIN_EXPORT itkINRDataImageReaderPlugin : public dtkPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inria.itkINRDataImageReaderPlugin" FILE "itkINRDataImageReaderPlugin.json")
    Q_INTERFACES(dtkPlugin)

public:
     itkINRDataImageReaderPlugin(QObject *parent = 0);
    ~itkINRDataImageReaderPlugin();

    virtual bool initialize();
    virtual bool uninitialize();

    virtual QString name() const;
    virtual QString description() const;
    virtual QString version() const;
    virtual QString contact() const;
    virtual QStringList authors() const;


    virtual QStringList tags() const;
    virtual QStringList types() const;
};
