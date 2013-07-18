/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDataSHImageReaderPlugin.h>
#include <dtkLog/dtkLog.h>
#include <dtkItkLogForwarder/itkLogForwarder.h>

#include <itkNrrdDataSHImageReader.h>
#include <itkNiftiDataSHImageReader.h>
#include <itkMetaDataSHImageReader.h>

// /////////////////////////////////////////////////////////////////
// itkDataSHImageReaderPluginPrivate
// /////////////////////////////////////////////////////////////////

class itkDataSHImageReaderPluginPrivate {
public:
    itkDataSHImageReaderPluginPrivate() {
        forwarder = itk::LogForwarder::New();
    }
    ~itkDataSHImageReaderPluginPrivate() { }
    // Class variables go here.
    itk::LogForwarder::Pointer forwarder;
};

// /////////////////////////////////////////////////////////////////
// itkDataSHImageReaderPlugin
// /////////////////////////////////////////////////////////////////

itkDataSHImageReaderPlugin::itkDataSHImageReaderPlugin(QObject *parent): dtkPlugin(parent),d(new itkDataSHImageReaderPluginPrivate) { }

itkDataSHImageReaderPlugin::~itkDataSHImageReaderPlugin() {
    delete d;
    d = NULL;
}

bool itkDataSHImageReaderPlugin::initialize() {
    if (!itkMetaDataSHImageReader::registered())  { dtkWarn() << "Unable to register itkMetaDataSHImageReader type";  }
    if (!itkNiftiDataSHImageReader::registered()) { dtkWarn() << "Unable to register itkNiftiDataSHImageReader type"; }
    if (!itkNrrdDataSHImageReader::registered())  { dtkWarn() << "Unable to register itkNrrdDataSHImageReader type";  }

    return true;
}

bool itkDataSHImageReaderPlugin::uninitialize() {
    return true;
}

QString itkDataSHImageReaderPlugin::name() const {
    return "itkDataSHImageReaderPlugin";
}

QString itkDataSHImageReaderPlugin::contact() const {
    return QString::fromUtf8("Théo Papadopoulo");
}

QStringList itkDataSHImageReaderPlugin::authors() const {
    QStringList list;
    list << QString::fromUtf8("Jaime Garcia Guevara");
    return list;
}

QStringList itkDataSHImageReaderPlugin::contributors() const {
    QStringList list;
    list << QString::fromUtf8("Théo Papadopoulo");
    return list;
}

QString itkDataSHImageReaderPlugin::version() const {
    return ITKDATASHIMAGEREADERPLUGIN_VERSION;
}

QString itkDataSHImageReaderPlugin::description() const {
    return "ITK SH data image readers";
}

QStringList itkDataSHImageReaderPlugin::tags() const {
    return QStringList() << "itk" << "data" << "SH" << "image" << "reader";
}

QStringList itkDataSHImageReaderPlugin::types() const {
    return QStringList() << "itkMetaDataSHImageReader"
                         << "itkNiftiDataSHImageReader"
                         << "itkNrrdDataSHImageReader";
}

Q_EXPORT_PLUGIN2(itkDataSHImageReaderPlugin, itkDataSHImageReaderPlugin)
