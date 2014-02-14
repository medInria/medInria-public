/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "v3dDataPlugin.h"

#include <dtkLog/dtkLog.h>

#include <dtkVtkLogForwarder/vtkLogForwarder.h>

#include <itkNiftiImageIOFactory.h>
#include <itkGDCMImageIOFactory.h>
#include <itkVTKImageIOFactory.h>

#include <v3dDataImage.h>
#include <v3dDataFibers.h>
#include <v3dDataFibersWriter.h>
#include <v3dDataFibersReader.h>

// /////////////////////////////////////////////////////////////////
// v3dDataPluginPrivate
// /////////////////////////////////////////////////////////////////

class v3dDataPluginPrivate
{
public:
    v3dDataPluginPrivate()
    {
        forwarder = vtkLogForwarder::New();
    }
    ~v3dDataPluginPrivate()
    {
        forwarder->Delete();
    }
    // Class variables go here.
    vtkLogForwarder* forwarder;
};

// /////////////////////////////////////////////////////////////////
// v3dDataPlugin
// /////////////////////////////////////////////////////////////////

v3dDataPlugin::v3dDataPlugin(QObject *parent) : dtkPlugin(parent), d(new v3dDataPluginPrivate)
{

}

v3dDataPlugin::~v3dDataPlugin()
{
    delete d;

    d = NULL;
}

bool v3dDataPlugin::initialize()
{
    itk::GDCMImageIOFactory::RegisterOneFactory();
    itk::NiftiImageIOFactory::RegisterOneFactory();
    itk::VTKImageIOFactory::RegisterOneFactory();

    if (!v3dDataImage::registered()) {
        dtkWarn() << "Unable to register v3dDataImage type";
    }

    if (!v3dDataFibers::registered()) {
        dtkWarn() << "Unable to register v3dDataFibers type";
    }

    if (!v3dDataFibersWriter::registered()) {
        dtkWarn() << "Unable to register v3dDataFibersWriter type";
    }

    if (!v3dDataFibersReader::registered()) {
        dtkWarn() << "Unable to register v3dDataFibersReader type";
    }

    return true;
}

bool v3dDataPlugin::uninitialize()
{
    return true;
}

QString v3dDataPlugin::name() const
{
    return "v3dDataPlugin";
}

QString v3dDataPlugin::description() const
{
    return "v3dData";
}

QString v3dDataPlugin::version() const
{
    return V3DDATAPLUGIN_VERSION;
}

QStringList v3dDataPlugin::tags() const
{
    return QStringList() << "v3d" << "data";
}



QStringList v3dDataPlugin::types() const
{
    return QStringList() << "v3dDataImage"
                         << "v3dDataFibers"
                         << "v3dDataFibersWriter"
                         << "v3dDataFibersReader";
}

Q_EXPORT_PLUGIN2(v3dDataPlugin, v3dDataPlugin)
