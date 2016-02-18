#pragma once

#include <dtkCore/dtkPlugin.h>
#include <mscAlgorithmPaintPluginExport.h>

namespace msc
{

class MSCALGORITHMPAINT_EXPORT AlgorithmPaintPlugin : public dtkPlugin
{

    Q_OBJECT
    Q_INTERFACES(dtkPlugin)

public:

    AlgorithmPaintPlugin(QObject *parent = 0);

    virtual bool initialize();
    virtual bool uninitialize();

    virtual QString name() const;
    virtual QString identifier() const;
    virtual QString description() const;
    virtual QString contact() const;
    virtual QString version() const;
    virtual QStringList authors() const;
    virtual QStringList contributors() const;
    virtual QStringList dependencies() const;

    virtual QStringList tags() const;
    virtual QStringList types() const;

};

}
