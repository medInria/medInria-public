/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkFibersDataPlugin.h>

#include <dtkLog/dtkLog.h>

#include <vtkLogForwarder.h>

#include <medVtkFibersData.h>
#include <medVtkFibersDataWriter.h>
#include <medVtkFibersDataReader.h>
#include <medVtkViewVtkFibersInteractor.h>

// /////////////////////////////////////////////////////////////////
// medVtkFibersDataPluginPrivate
// /////////////////////////////////////////////////////////////////

class medVtkFibersDataPluginPrivate
{
public:
    medVtkFibersDataPluginPrivate()
    {
        forwarder = vtkLogForwarder::New();
    }
    ~medVtkFibersDataPluginPrivate()
    {
        forwarder->Delete();
    }
    // Class variables go here.
    vtkLogForwarder* forwarder;
};

// /////////////////////////////////////////////////////////////////
// medVtkFibersDataPlugin
// /////////////////////////////////////////////////////////////////

medVtkFibersDataPlugin::medVtkFibersDataPlugin(QObject *parent) : dtkPlugin(parent), d(new medVtkFibersDataPluginPrivate)
{

}

medVtkFibersDataPlugin::~medVtkFibersDataPlugin()
{
    delete d;

    d = NULL;
}

bool medVtkFibersDataPlugin::initialize()
{
    if (!medVtkFibersData::registered()) {
        dtkWarn() << "Unable to register medVtkFibersData type";
    }

    if (!medVtkFibersDataWriter::registered()) {
        dtkWarn() << "Unable to register medVtkFibersDataWriter type";
    }

    if (!medVtkFibersDataReader::registered()) {
        dtkWarn() << "Unable to register medVtkFibersDataReader type";
    }

    if (!medVtkViewVtkFibersInteractor::registered()) {
        dtkWarn() << "Unable to register medVtkViewVtkFibersInteractor";
    }

    return true;
}

bool medVtkFibersDataPlugin::uninitialize()
{
    return true;
}

QString medVtkFibersDataPlugin::name() const
{
    return "medVtkFibersDataPlugin";
}

QString medVtkFibersDataPlugin::description() const
{
    return "medVtkFibersData";
}

QString medVtkFibersDataPlugin::version() const
{
    return MEDVTKFIBERSDATAPLUGIN_VERSION;
}

QStringList medVtkFibersDataPlugin::tags() const
{
    return QStringList() << "vtk" << "data";
}



QStringList medVtkFibersDataPlugin::types() const
{
    return QStringList() << "medVtkFibersData"
                         << "medVtkFibersDataWriter"
                         << "medVtkFibersDataReader";
}

Q_EXPORT_PLUGIN2(medVtkFibersDataPlugin, medVtkFibersDataPlugin)
