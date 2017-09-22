#include "mscAlgorithmPaintPlugin.h"

#include <dtkLog/dtkLog.h>
#include <mscAlgorithmPaintToolBox.h>

namespace msc
{

AlgorithmPaintPlugin::AlgorithmPaintPlugin(QObject *parent) : medPlugin(parent)
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

QString AlgorithmPaintPlugin::name() const
{
    return "Algorithm Paint";
}

QString AlgorithmPaintPlugin::description() const
{
    QString description = \
            tr("Segmentation using manual painting. \
               <br><br>This process is based on the <a href=\"https://itk.org/\" style=\"color: #cc0000\" >ITK library</a>.");
    return description;
}

QString AlgorithmPaintPlugin::version() const
{
    return MSCALGORITHMPAINT_VERSION;
}

QStringList AlgorithmPaintPlugin::types() const
{
    return QStringList() << "Segmentation";
}

Q_EXPORT_PLUGIN2(mscAlgorithmPaint, AlgorithmPaintPlugin)

}
