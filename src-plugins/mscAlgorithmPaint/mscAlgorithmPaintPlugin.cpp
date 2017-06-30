#include "mscAlgorithmPaintPlugin.h"

#include <dtkLog/dtkLog.h>
#include <mscAlgorithmPaintToolBox.h>

namespace msc
{

bool AlgorithmPaintPlugin::initialize()
{
    if(!AlgorithmPaintToolBox::registered())
    {
        dtkWarn() << "Unable to register AlgorithmPaintToolBox";
    }
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

QString AlgorithmPaintPlugin::identifier() const
{
    return "mscAlgorithmPaint";
}

QStringList AlgorithmPaintPlugin::types() const
{
    return QStringList() << "Segmentation";
}

Q_EXPORT_PLUGIN2(mscAlgorithmPaint, AlgorithmPaintPlugin)

}
