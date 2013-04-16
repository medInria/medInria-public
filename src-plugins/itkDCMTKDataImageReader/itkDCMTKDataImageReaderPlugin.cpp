/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/




#include "itkDCMTKDataImageReader.h"
#include "itkDCMTKDataImageReaderPlugin.h"

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// itkDCMTKDataImageReaderPluginPrivate
// /////////////////////////////////////////////////////////////////

class itkDCMTKDataImageReaderPluginPrivate
{
public:
    // Class variables go here.
};

// /////////////////////////////////////////////////////////////////
// itkDCMTKDataImageReaderPlugin
// /////////////////////////////////////////////////////////////////

itkDCMTKDataImageReaderPlugin::itkDCMTKDataImageReaderPlugin(QObject *parent) : dtkPlugin(parent), d(new itkDCMTKDataImageReaderPluginPrivate)
{

}

itkDCMTKDataImageReaderPlugin::~itkDCMTKDataImageReaderPlugin()
{
    delete d;

    d = NULL;
}

bool itkDCMTKDataImageReaderPlugin::initialize()
{
    if (!itkDCMTKDataImageReader::registered()) { dtkWarn() << "Unable to register itkDCMTKDataImageReader type"; }

    return true;
}

bool itkDCMTKDataImageReaderPlugin::uninitialize()
{
    return true;
}

QString itkDCMTKDataImageReaderPlugin::name() const
{
    return "itkDCMTKDataImageReaderPlugin";
}

QString itkDCMTKDataImageReaderPlugin::description() const
{
    return "ITK DICOM reader using DCMTK";
}

QStringList itkDCMTKDataImageReaderPlugin::authors() const
{
    QStringList list;
    list << QString::fromUtf8("Pierre Fillard");
    return list;
}

QStringList itkDCMTKDataImageReaderPlugin::contributors() const
{
    QStringList list;
    list <<  "Michael Knopke";
    return list;
}


QString itkDCMTKDataImageReaderPlugin::version() const
{
    return ITKDCMTKDATAIMAGEREADERPLUGIN_VERSION;
}


QStringList itkDCMTKDataImageReaderPlugin::tags() const
{
    return QStringList() << "itk" << "dcmtk" << "data" << "image" << "reader";
}

QStringList itkDCMTKDataImageReaderPlugin::types() const
{
    return QStringList() << "itkDCMTKDataImageReader";
}

Q_EXPORT_PLUGIN2(itkDCMTKDataImageReaderPlugin, itkDCMTKDataImageReaderPlugin)
