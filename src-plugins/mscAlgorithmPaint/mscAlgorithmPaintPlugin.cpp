#include "mscAlgorithmPaintPlugin.h"

#include <dtkLog/dtkLog.h>
#include <mscAlgorithmPaintToolBox.h>

namespace msc
{

AlgorithmPaintPlugin::AlgorithmPaintPlugin(QObject *parent) : dtkPlugin(parent)
{
}

bool AlgorithmPaintPlugin::initialize()
{
    if(!AlgorithmPaintToolBox::registered())
    {
        dtkWarn() << "Unable to register AlgorithmPaintToolBox";
    }
    return true;
}

bool AlgorithmPaintPlugin::uninitialize()
{
    return true;
}

QString AlgorithmPaintPlugin::name() const
{
    return "Algorithm Paint";
}

QString AlgorithmPaintPlugin::description() const
{
    return tr("Segmentation using manual painting");
}

QString AlgorithmPaintPlugin::version() const
{
    return MSCALGORITHMPAINT_VERSION;
}

QString AlgorithmPaintPlugin::contact() const
{
    return "";
}

QStringList AlgorithmPaintPlugin::authors() const
{
    QStringList list;
    return list;
}

QStringList AlgorithmPaintPlugin::contributors() const
{
    QStringList list;
    return list;
}

QString AlgorithmPaintPlugin::identifier() const
{
    return "mscAlgorithmPaint";
}

QStringList AlgorithmPaintPlugin::tags() const
{
    return QStringList();
}

QStringList AlgorithmPaintPlugin::types() const
{
    return QStringList() << "segmentation";
}

QStringList AlgorithmPaintPlugin::dependencies() const
{
    return QStringList();
}

Q_EXPORT_PLUGIN2(mscAlgorithmPaint, AlgorithmPaintPlugin)

}
