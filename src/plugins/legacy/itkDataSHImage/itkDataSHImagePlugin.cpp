/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDataSHImagePlugin.h>

#include <itkDataSHImageDouble3.h>
#include <itkDataSHImageFloat3.h>
#include <itkDataSHImageVtkViewInteractor.h>

#include <itkDataSHImageReader.h>
#include <itkMetaDataSHImageWriter.h>

#include <dtkLog/dtkLog.h>
#include <itkLogForwarder.h>

// /////////////////////////////////////////////////////////////////
// itkDataSHImagePluginPrivate
// /////////////////////////////////////////////////////////////////

class itkDataSHImagePluginPrivate {
public:

    itkDataSHImagePluginPrivate() { forwarder = itk::LogForwarder::New(); }
    ~itkDataSHImagePluginPrivate() { }

    // Class variables go here.

    itk::LogForwarder::Pointer forwarder;
};

// /////////////////////////////////////////////////////////////////
// itkDataSHImagePlugin
// /////////////////////////////////////////////////////////////////

itkDataSHImagePlugin::itkDataSHImagePlugin(QObject *parent): dtkPlugin(parent),d(new itkDataSHImagePluginPrivate) { }

itkDataSHImagePlugin::~itkDataSHImagePlugin() {
    delete d;
    d = NULL;
}

bool itkDataSHImagePlugin::initialize() {
    if (!itkDataSHImageDouble3::registered()) { dtkWarn() << "Unable to register itkDataSHImageDouble3 type"; }
    if (!itkDataSHImageFloat3::registered())  { dtkWarn() << "Unable to register itkDataSHImageFloat3 type";  }
    if (!itkDataSHImageVtkViewInteractor::registered())  { dtkWarn() << "Unable to register itkDataSHImageVtkViewInteractor type";  }

    if (!itkDataSHImageReader::registered())  { dtkWarn() << "Unable to register itkDataSHImageReader type";  }
    if (!itkMetaDataSHImageWriter::registered())  { dtkWarn() << "Unable to register itkMetaDataSHImageWriter type"; }

    return true;
}

bool itkDataSHImagePlugin::uninitialize() {
    return true;
}

QString itkDataSHImagePlugin::name() const {
    return "itkDataSHImagePlugin";
}

QString itkDataSHImagePlugin::contact() const {
    return QString::fromUtf8("Théo Papadopoulo <Theodore.Papadopoulo@inria.fr>");
}

QStringList itkDataSHImagePlugin::authors() const {
    QStringList list;
    list << QString::fromUtf8("Jaime Garcia Guevara");
    return list;
}

QStringList itkDataSHImagePlugin::contributors() const {
    QStringList list;
    list << QString::fromUtf8("Théo Papadopoulo");
    return list;
}

QString itkDataSHImagePlugin::version() const {
    return ITKDATASHIMAGEPLUGIN_VERSION;
}

QString itkDataSHImagePlugin::description() const {
    return "Handle ITK SH images and their visualization. <br/> See  <a href=\"https://doi.org/10.1002/mrm.21277 \">https://doi.org/10.1002/mrm.21277 </a>.";
}

QStringList itkDataSHImagePlugin::tags() const {
    return QStringList() << "itk" << "data" << "SH" << "image" << "reader" << "writer";
}

QStringList itkDataSHImagePlugin::types() const {
    return QStringList() << "itkDataSHImageFloat3"
                         << "itkDataSHImageDouble3"
                         << "itkDataSHImageReader"
                         << "itkMetaDataSHImageWriter";
}

//Q_EXPORT_PLUGIN2(itkDataSHImagePlugin, itkDataSHImagePlugin)
