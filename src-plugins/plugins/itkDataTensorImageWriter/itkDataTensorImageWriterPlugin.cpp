/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkMetaDataTensorImageWriter.h"
#include "itkNiftiDataTensorImageWriter.h"
#include "itkNrrdDataTensorImageWriter.h"
#include "itkDataTensorImageWriterPlugin.h"

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// itkDataTensorImageWriterPluginPrivate
// /////////////////////////////////////////////////////////////////

class itkDataTensorImageWriterPluginPrivate
{
public:
    // Class variables go here.
};

// /////////////////////////////////////////////////////////////////
// itkDataTensorImageWriterPlugin
// /////////////////////////////////////////////////////////////////

itkDataTensorImageWriterPlugin::itkDataTensorImageWriterPlugin(QObject *parent) : dtkPlugin(parent), d(new itkDataTensorImageWriterPluginPrivate)
{

}

itkDataTensorImageWriterPlugin::~itkDataTensorImageWriterPlugin()
{
    delete d;

    d = NULL;
}

bool itkDataTensorImageWriterPlugin::initialize()
{
    if (!itkMetaDataTensorImageWriter::registered())  { dtkWarn() << "Unable to register itkMetaDataTensorImageWriter type";  }
    if (!itkNiftiDataTensorImageWriter::registered()) { dtkWarn() << "Unable to register itkNiftiDataTensorImageWriter type"; }
    if (!itkNrrdDataTensorImageWriter::registered())  { dtkWarn() << "Unable to register itkNrrdDataTensorImageWriter type";  }

    return true;
}

bool itkDataTensorImageWriterPlugin::uninitialize()
{
    return true;
}

QString itkDataTensorImageWriterPlugin::name() const
{
    return "itkDataTensorImageWriterPlugin";
}

QString itkDataTensorImageWriterPlugin::description() const
{
    return "ITK tensor data image writers";
}

QString itkDataTensorImageWriterPlugin::version() const
{
    return ITKDATATENSORIMAGEWRITERPLUGIN_VERSION;
}

QStringList itkDataTensorImageWriterPlugin::tags() const
{
    return QStringList() << "itk" << "data" << "tensor" << "image" << "writers";
}

QStringList itkDataTensorImageWriterPlugin::types() const
{
    return QStringList() << "itkMetaDataTensorImageWriter"
                         << "itkNiftiDataTensorImageWriter"
                         << "itkNrrdDataTensorImageWriter";
}

Q_EXPORT_PLUGIN2(itkDataTensorImageWriterPlugin, itkDataTensorImageWriterPlugin)
