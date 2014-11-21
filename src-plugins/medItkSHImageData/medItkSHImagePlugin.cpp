/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkSHImagePlugin.h>

#include <medItkDouble3SHImageData.h>
#include <medItkFloat3SHImageData.h>
#include <medVtkViewItkSHImageInteractor.h>

#include <medItkSHImageDataNrrdReader.h>
#include <medItkSHImageDataNiftiReader.h>
#include <medItkSHImageMetaDataReader.h>

#include <medItkSHImageMetaDataWriter.h>

#include <dtkLog/dtkLog.h>
#include <itkLogForwarder.h>

// /////////////////////////////////////////////////////////////////
// medItkSHImagePluginPrivate
// /////////////////////////////////////////////////////////////////

class medItkSHImagePluginPrivate {
public:

    medItkSHImagePluginPrivate() { forwarder = itk::LogForwarder::New(); }
    ~medItkSHImagePluginPrivate() { }

    // Class variables go here.

    itk::LogForwarder::Pointer forwarder;
};

// /////////////////////////////////////////////////////////////////
// medItkSHImagePlugin
// /////////////////////////////////////////////////////////////////

medItkSHImagePlugin::medItkSHImagePlugin(QObject *parent): dtkPlugin(parent),d(new medItkSHImagePluginPrivate) { }

medItkSHImagePlugin::~medItkSHImagePlugin() {
    delete d;
    d = NULL;
}

bool medItkSHImagePlugin::initialize() {
    if (!medItkDouble3SHImageData::registered()) { dtkWarn() << "Unable to register medItkDouble3SHImageData type"; }
    if (!medItkFloat3SHImageData::registered())  { dtkWarn() << "Unable to register medItkFloat3SHImageData type";  }
    if (!medVtkViewItkSHImageInteractor::registered())  { dtkWarn() << "Unable to register medVtkViewItkSHImageInteractor type";  }

    if (!medItkSHImageMetaDataReader::registered())  { dtkWarn() << "Unable to register medItkSHImageMetaDataReader type";  }
    if (!medItkSHImageDataNiftiReader::registered()) { dtkWarn() << "Unable to register medItkSHImageDataNiftiReader type"; }
    if (!medItkSHImageDataNrrdReader::registered())  { dtkWarn() << "Unable to register medItkSHImageDataNrrdReader type";  }

    if(!medItkSHImageMetaDataWriter::registered())  { dtkWarn() << "Unable to register medItkSHImageMetaDataWriter type"; }

    return true;
}

bool medItkSHImagePlugin::uninitialize() {
    return true;
}

QString medItkSHImagePlugin::name() const {
    return "medItkSHImagePlugin";
}

QString medItkSHImagePlugin::contact() const {
    return QString::fromUtf8("Théo Papadopoulo <Theodore.Papadopoulo@inria.fr>");
}

QStringList medItkSHImagePlugin::authors() const {
    QStringList list;
    list << QString::fromUtf8("Jaime Garcia Guevara");
    return list;
}

QStringList medItkSHImagePlugin::contributors() const {
    QStringList list;
    list << QString::fromUtf8("Théo Papadopoulo");
    return list;
}

QString medItkSHImagePlugin::version() const {
    return MEDITKSHIMAGEPLUGIN_VERSION;
}

QString medItkSHImagePlugin::description() const {
    return "Handle ITK SH images and their visualization. <br/> See  <a href=\"http://dx.doi.org/10.1002/mrm.21277 \">http://dx.doi.org/10.1002/mrm.21277 </a>.";
}

QStringList medItkSHImagePlugin::tags() const {
    return QStringList() << "itk" << "data" << "SH" << "image" << "reader" << "writer";
}

QStringList medItkSHImagePlugin::types() const {
    return QStringList() << "medItkFloat3SHImageData"
                         << "medItkDouble3SHImageData"
                         << "medItkSHImageMetaDataReader"
                         << "medItkSHImageDataNiftiReader"
                         << "medItkSHImageDataNrrdReader"
                         << "medItkSHImageMetaDataWriter";
}

Q_EXPORT_PLUGIN2(medItkSHImagePlugin, medItkSHImagePlugin)
