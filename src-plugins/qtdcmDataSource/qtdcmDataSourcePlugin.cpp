/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <qtdcmDataSource.h>
#include <qtdcmDataSourcePlugin.h>

qtdcmDataSourcePlugin::qtdcmDataSourcePlugin(QObject *parent) : medPlugin(parent)
{
}

bool qtdcmDataSourcePlugin::initialize()
{
    if ( !qtdcmDataSource::registered() ) { dtkWarn() << "Unable to register qtdcmDataSource type"; }

    return true;
}

QString qtdcmDataSourcePlugin::name() const
{
    return "QtDCM Data Source";
}

QString qtdcmDataSourcePlugin::description() const
{
    return "This plugin adds Dicom functionality to the medInria browser area";
}

QStringList qtdcmDataSourcePlugin::authors() const
{
  return QStringList() << "Alexandre Abadie ";
}

QStringList qtdcmDataSourcePlugin::contributors() const
{
  return QStringList() << "Olivier Commowick"
                       << medPlugin::contributors();
}

QStringList qtdcmDataSourcePlugin::dependencies() const
{
  return QStringList() << "QtDcm v2.0";
}

QString qtdcmDataSourcePlugin::version() const
{
  return QTDCMDATASOURCEPLUGIN_VERSION;
}

QStringList qtdcmDataSourcePlugin::tags() const
{
    return QStringList() << "dicom" << "data" << "source";
}

QStringList qtdcmDataSourcePlugin::types() const
{
    return QStringList() << "qtdcmDataSource";
}

Q_EXPORT_PLUGIN2 ( qtdcmDataSourcePlugin, qtdcmDataSourcePlugin )
