/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkMetaDataSHImageWriter.h>
#include <itkDataSHImageWriterPlugin.h>

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// itkDataSHImageWriterPluginPrivate
// /////////////////////////////////////////////////////////////////

class itkDataSHImageWriterPluginPrivate
{
public:
    // Class variables go here.
};

// /////////////////////////////////////////////////////////////////
// itkDataSHImageWriterPlugin
// /////////////////////////////////////////////////////////////////

itkDataSHImageWriterPlugin::itkDataSHImageWriterPlugin(QObject *parent) : dtkPlugin(parent), d(new itkDataSHImageWriterPluginPrivate)
{

}

itkDataSHImageWriterPlugin::~itkDataSHImageWriterPlugin()
{
    delete d;

    d = NULL;
}

bool itkDataSHImageWriterPlugin::initialize()
{
    if(!itkMetaDataSHImageWriter::registered())
        dtkWarn() << "Unable to register itkMetaDataSHImageWriter type";

    return true;
}

bool itkDataSHImageWriterPlugin::uninitialize()
{
    return true;
}

QString itkDataSHImageWriterPlugin::name() const
{
    return "itkDataSHImageWriterPlugin";
}

QString itkDataSHImageWriterPlugin::description() const
{
    return "ITK SH data image writers";
}

QStringList itkDataSHImageWriterPlugin::tags() const
{
    return QStringList() << "itk" << "data" << "SH" << "image" << "writers";
}

QStringList itkDataSHImageWriterPlugin::types() const
{
    return QStringList() << "itkMetaDataSHImageWriter";
}

Q_EXPORT_PLUGIN2(itkDataSHImageWriterPlugin, itkDataSHImageWriterPlugin)
