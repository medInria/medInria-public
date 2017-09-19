#pragma once

#include <medPlugin.h>
#include <mscAlgorithmPaintPluginExport.h>

namespace msc
{

class MSCALGORITHMPAINT_EXPORT AlgorithmPaintPlugin : public medPlugin
{
    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:
    AlgorithmPaintPlugin(QObject *parent = 0);
    virtual bool initialize();

    virtual QString name() const;
    virtual QString description() const;
    virtual QString version() const;
    virtual QStringList types() const;
};

}
