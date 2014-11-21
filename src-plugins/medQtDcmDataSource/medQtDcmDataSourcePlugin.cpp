/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medQtDcmDataSource.h>
#include <medQtDcmDataSourcePlugin.h>

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// medQtDcmDataSourcePluginPrivate
// /////////////////////////////////////////////////////////////////

class medQtDcmDataSourcePluginPrivate
{
public:
    // Class variables go here.
};

// /////////////////////////////////////////////////////////////////
// medQtDcmDataSourcePlugin
// /////////////////////////////////////////////////////////////////

medQtDcmDataSourcePlugin::medQtDcmDataSourcePlugin ( QObject *parent ) : dtkPlugin ( parent ), d ( new medQtDcmDataSourcePluginPrivate )
{

}

medQtDcmDataSourcePlugin::~medQtDcmDataSourcePlugin()
{
    delete d;

    d = NULL;
}

bool medQtDcmDataSourcePlugin::initialize()
{
    if ( !medQtDcmDataSource::registered() ) { dtkWarn() << "Unable to register medQtDcmDataSource type"; }

    return true;
}

bool medQtDcmDataSourcePlugin::uninitialize()
{
    return true;
}

QString medQtDcmDataSourcePlugin::name() const
{
    return "medQtDcmDataSourcePlugin";
}

QString medQtDcmDataSourcePlugin::description() const
{
    return "This plugin adds Dicom functionality to the medInria browser area";
}

QStringList medQtDcmDataSourcePlugin::authors() const
{
  return QStringList() << "Alexandre Abadie <Alexandre.Abadie@inria.fr>";
}

QString medQtDcmDataSourcePlugin::contact() const
{
  return "Alexandre Abadie <Alexandre.Abadie@inria.fr>";
}

QStringList medQtDcmDataSourcePlugin::contributors() const
{
  return QStringList() << "Olivier Commowick <Olivier.Commowick@inria.fr>";
}

QStringList medQtDcmDataSourcePlugin::dependencies() const
{
  return QStringList() << "QtDcm v2.0";
}

QString medQtDcmDataSourcePlugin::version() const
{
  return MEDQTDCMDATASOURCEPLUGIN_VERSION;
}


QStringList medQtDcmDataSourcePlugin::tags() const
{
    return QStringList() << "dicom" << "data" << "source";
}

QStringList medQtDcmDataSourcePlugin::types() const
{
    return QStringList() << "medQtDcmDataSource";
}

Q_EXPORT_PLUGIN2 ( medQtDcmDataSourcePlugin, medQtDcmDataSourcePlugin )
