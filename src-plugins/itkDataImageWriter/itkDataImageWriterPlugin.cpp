/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkDataImageWriterPlugin.h"
#include "itkMetaDataImageWriter.h"
#include "itkNiftiDataImageWriter.h"
#include "itkNrrdDataImageWriter.h"
#include "itkGiplDataImageWriter.h"
#include "itkVTKDataImageWriter.h"
#include "itkPhilipsRECDataImageWriter.h"
#include "itkGISDataImageWriter.h"

#include <dtkLog/dtkLog.h>
#include <dtkItkLogForwarder/itkLogForwarder.h>

// /////////////////////////////////////////////////////////////////
// itkDataImageWriterPluginPrivate
// /////////////////////////////////////////////////////////////////

class itkDataImageWriterPluginPrivate
{
public:
    itkDataImageWriterPluginPrivate()
    {
        forwarder = itk::LogForwarder::New();
    }

    ~itkDataImageWriterPluginPrivate()
    {
    }

    // Class variables go here.
    itk::LogForwarder::Pointer forwarder;
};

// /////////////////////////////////////////////////////////////////
// itkDataImageWriterPlugin
// /////////////////////////////////////////////////////////////////

itkDataImageWriterPlugin::itkDataImageWriterPlugin(QObject *parent)
    : dtkPlugin(parent)
    , d(new itkDataImageWriterPluginPrivate)
{
}

itkDataImageWriterPlugin::~itkDataImageWriterPlugin()
{
    delete d;
    d = NULL;
}

bool itkDataImageWriterPlugin::initialize()
{
    if (!itkMetaDataImageWriter::registered())       { dtkWarn() << "Unable to register itkMetaDataImageWriter type";       }
    if (!itkNiftiDataImageWriter::registered())      { dtkWarn() << "Unable to register itkNiftiDataImageWriter type";      }
    if (!itkNrrdDataImageWriter::registered())       { dtkWarn() << "Unable to register itkNrrdDataImageWriter type";       }
    if (!itkGiplDataImageWriter::registered())       { dtkWarn() << "Unable to register itkGiplDataImageWriter type";       }
    if (!itkVTKDataImageWriter::registered())        { dtkWarn() << "Unable to register itkVTKDataImageWriter type";        }
    if (!itkPhilipsRECDataImageWriter::registered()) { dtkWarn() << "Unable to register itkPhilipsRECDataImageWriter type"; }
    if (!itkGISDataImageWriter::registered())        { dtkWarn() << "Unable to register itkGISDataImageWriter type"; }

    return true;
}

bool itkDataImageWriterPlugin::uninitialize()
{
    return true;
}

QString itkDataImageWriterPlugin::name() const
{
    return "itkDataImageWriterPlugin";
}

QString itkDataImageWriterPlugin::description() const
{
    return "Writers for any ITK image data type.";
}

QString itkDataImageWriterPlugin::version() const
{
    return ITKDATAIMAGEWRITERPLUGIN_VERSION;
}


QStringList itkDataImageWriterPlugin::tags() const
{
    return QStringList() << "itk" << "data" << "image" << "writer";
}

QStringList itkDataImageWriterPlugin::types() const
{
    return QStringList() << "itkMetaDataImageWriter"
                         << "itkNiftiDataImageWriter"
                         << "itkAnalyzeDataImageWriter"
                         << "itkNrrdDataImageWriter"
                         << "itkGiplDataImageWriter"
                         << "itkVTKDataImageWriter"
                         << "itkGISDataImageWriter";
}

Q_EXPORT_PLUGIN2(itkDataImageWriterPlugin, itkDataImageWriterPlugin)
