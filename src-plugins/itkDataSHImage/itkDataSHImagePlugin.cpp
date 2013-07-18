/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDataSHImagePlugin.h>

#include <itkDataSHImageDouble3.h>
#include <itkDataSHImageFloat3.h>

#include <dtkLog/dtkLog.h>
#include <dtkItkLogForwarder/itkLogForwarder.h>

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

    return true;
}

bool itkDataSHImagePlugin::uninitialize() {
    return true;
}

QString itkDataSHImagePlugin::name() const {
    return "itkDataSHImagePlugin";
}

QString itkDataSHImagePlugin::contact() const {
    return QString::fromUtf8("Théo Papadopoulo");
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
    return "Handle ITK SH image types";
}

QStringList itkDataSHImagePlugin::tags() const {
    return QStringList() << "itk" << "data" << "SH" << "image";
}

QStringList itkDataSHImagePlugin::types() const {
    return QStringList() << "itkDataSHImageFloat3" << "itkDataSHImageDouble3";
}

Q_EXPORT_PLUGIN2(itkDataSHImagePlugin, itkDataSHImagePlugin)
