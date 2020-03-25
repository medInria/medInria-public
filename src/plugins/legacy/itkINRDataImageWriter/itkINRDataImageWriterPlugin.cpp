#include "itkINRDataImageWriter.h"
#include "itkINRDataImageWriterPlugin.h"

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// itkINRDataImageWriterPlugin
// /////////////////////////////////////////////////////////////////

itkINRDataImageWriterPlugin::itkINRDataImageWriterPlugin(QObject *parent) : dtkPlugin(parent)
{
}

itkINRDataImageWriterPlugin::~itkINRDataImageWriterPlugin()
{
}

bool itkINRDataImageWriterPlugin::initialize()
{
    if(!itkINRDataImageWriter::registered())
    {
        dtkWarn() << "Unable to register itkINRDataImageWriter type";
    }

    return true;
}

bool itkINRDataImageWriterPlugin::uninitialize()
{
    return true;
}

QString itkINRDataImageWriterPlugin::name() const
{
    return "itkINRDataImageWriterPlugin";
}

QString itkINRDataImageWriterPlugin::contact() const
{
    return QString::fromUtf8("benoit.bleuze@inria.fr");
}

QStringList itkINRDataImageWriterPlugin::authors() const
{
    QStringList list;
    list << QString::fromUtf8("Benoît Bleuzé");
    return list;
}

QString itkINRDataImageWriterPlugin::description() const
{
    return  tr("INRImage writer plugin");
}

QString itkINRDataImageWriterPlugin::version() const
{
    return ITKINRDATAIMAGEWRITERPLUGIN_VERSION;
}

QStringList itkINRDataImageWriterPlugin::tags() const
{
    return QStringList();
}

QStringList itkINRDataImageWriterPlugin::types() const
{
    return QStringList() << "itkINRDataImageWriter";
}
