/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkMeshPlugin.h>
#include <medVtkMeshData.h>
#include <medVtk4DMeshData.h>
#include <medVtkViewVtkMeshInteractor.h>
#include <medVtkViewVtk4DMeshInteractor.h>
#include <medVtkViewVtkMeshNavigator.h>

#include <medVtkMeshDataReader.h>
#include <medVtk4DMeshDataReader.h>

#include <medVtkMeshDataWriter.h>
#include <medVtk4DMeshDataWriter.h>

#include <dtkLog/dtkLog.h>

#include <vtkLogForwarder.h>

// /////////////////////////////////////////////////////////////////
// medVtkMeshPluginPrivate
// /////////////////////////////////////////////////////////////////

class medVtkMeshPluginPrivate
{
public:
  medVtkMeshPluginPrivate()
  {
      forwarder = vtkLogForwarder::New();
  }
  ~medVtkMeshPluginPrivate()
  {
      forwarder->Delete();
  }
  // Class variables go here.
  vtkLogForwarder* forwarder;
};

// /////////////////////////////////////////////////////////////////
// medVtkMeshPlugin
// /////////////////////////////////////////////////////////////////

medVtkMeshPlugin::medVtkMeshPlugin(QObject *parent) : dtkPlugin(parent), d(new medVtkMeshPluginPrivate)
{

}

medVtkMeshPlugin::~medVtkMeshPlugin()
{
    delete d;
    d = NULL;
}

bool medVtkMeshPlugin::initialize()
{
  if ( ! medVtkMeshData::registered()) { dtkWarn() << "Unable to register medVtkMeshData type"; }
  if ( ! medVtk4DMeshData::registered()) { dtkWarn() << "Unable to register medVtk4DMeshData type"; }
  if ( ! medVtkViewVtkMeshInteractor::registered()) { dtkWarn() << "Unable to register medVtkViewVtkMeshInteractor type"; }
  if ( ! medVtkViewVtk4DMeshInteractor::registered()) { dtkWarn() << "Unable to register medVtkViewVtk4DMeshInteractor type"; }
  if ( ! medVtkViewVtkMeshNavigator::registered()) { dtkWarn() << "Unable to register medVtkViewVtkMeshNavigator type"; }

  if ( ! medVtkMeshDataReader::registered())   { dtkWarn() << "Unable to register medVtkMeshDataReader type"; }
  if ( ! medVtk4DMeshDataReader::registered()) { dtkWarn() << "Unable to register medVtk4DMeshDataReader type"; }

  if ( ! medVtkMeshDataWriter::registered())   { dtkWarn() << "Unable to register medVtkMeshDataWriter type"; }
  if ( ! medVtk4DMeshDataWriter::registered()) { dtkWarn() << "Unable to register medVtk4DMeshDataWriter type"; }

  return true;
}

bool medVtkMeshPlugin::uninitialize()
{
    return true;
}

QString medVtkMeshPlugin::name() const
{
    return "medVtkMeshPlugin";
}

QString medVtkMeshPlugin::description() const
{
    return "Plugin containing all types of VTK meshes";
}

QString medVtkMeshPlugin::version() const
{
    return MEDVTKMESHPLUGIN_VERSION;
}

QStringList medVtkMeshPlugin::tags() const
{
  return QStringList() << "vtk" << "data" << "mesh" << "4D" << "reader" << "writer";
}

QStringList medVtkMeshPlugin::types() const
{
  return QStringList() << "medVtkMeshData"
                       << "medVtkMeshDataWriter"
                       << "medVtk4DMeshDataWriter"
                       << "medVtkMeshDataWriter"
                       << "medVtk4DMeshDataWriter";
}

Q_EXPORT_PLUGIN2(medVtkMeshPlugin, medVtkMeshPlugin)
