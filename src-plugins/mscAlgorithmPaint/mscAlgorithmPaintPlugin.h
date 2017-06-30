#pragma once

#include <medPlugin.h>
#include <mscAlgorithmPaintPluginExport.h>

namespace msc
{

class MSCALGORITHMPAINT_EXPORT AlgorithmPaintPlugin : public medPlugin
{
    Q_OBJECT

public:
    virtual bool initialize();

    virtual QString name() const;
    virtual QString identifier() const;
    virtual QString description() const;
    virtual QString version() const;
    virtual QStringList types() const;
};

}
