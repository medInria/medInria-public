/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkDataMeshPlugin.h>
#include <vtkDataMesh.h>
#include <vtkDataMesh4D.h>
#include <vtkDataMeshInteractor.h>
#include <vtkDataMesh4DInteractor.h>
#include <vtkDataMeshNavigator.h>

#include <vtkDataMeshReader.h>
#include <vtkDataMesh4DReader.h>

#include <vtkDataMeshWriter.h>
#include <vtkDataMesh4DWriter.h>

#include <vtkLogForwarder.h>

// /////////////////////////////////////////////////////////////////
// vtkDataMeshPluginPrivate
// /////////////////////////////////////////////////////////////////

class vtkDataMeshPluginPrivate
{
public:
  vtkDataMeshPluginPrivate()
  {
      forwarder = vtkLogForwarder::New();
  }
  ~vtkDataMeshPluginPrivate()
  {
      forwarder->Delete();
  }
  // Class variables go here.
  vtkLogForwarder* forwarder;
};

// /////////////////////////////////////////////////////////////////
// vtkDataMeshPlugin
// /////////////////////////////////////////////////////////////////

vtkDataMeshPlugin::vtkDataMeshPlugin(QObject *parent) : medPlugin(parent), d(new vtkDataMeshPluginPrivate)
{

}

vtkDataMeshPlugin::~vtkDataMeshPlugin()
{
    delete d;
    d = NULL;
}

bool vtkDataMeshPlugin::initialize()
{
  if ( ! vtkDataMesh::registered()) { dtkWarn() << "Unable to register vtkDataMesh type"; }
  if ( ! vtkDataMesh4D::registered()) { dtkWarn() << "Unable to register vtkDataMesh4D type"; }
  if ( ! vtkDataMeshInteractor::registered()) { dtkWarn() << "Unable to register vtkDataMeshInteractor type"; }
  if ( ! vtkDataMesh4DInteractor::registered()) { dtkWarn() << "Unable to register vtkDataMesh4DInteractor type"; }
  if ( ! vtkDataMeshNavigator::registered()) { dtkWarn() << "Unable to register vtkDataMeshNavigator type"; }
  
  if ( ! vtkDataMeshReader::registered())   { dtkWarn() << "Unable to register vtkDataMeshReader type"; }
  if ( ! vtkDataMesh4DReader::registered()) { dtkWarn() << "Unable to register vtkDataMesh4DReader type"; }
  
  if ( ! vtkDataMeshWriter::registered())   { dtkWarn() << "Unable to register vtkDataMeshWriter type"; }
  if ( ! vtkDataMesh4DWriter::registered()) { dtkWarn() << "Unable to register vtkDataMesh4DWriter type"; }
  
  return true;
}

QString vtkDataMeshPlugin::name() const
{
    return "VTK Data Mesh";
}

QString vtkDataMeshPlugin::description() const
{
    return "Plugin containing all types of VTK meshes";
}

QString vtkDataMeshPlugin::version() const
{
    return VTKDATAMESHPLUGIN_VERSION;
}

QStringList vtkDataMeshPlugin::tags() const
{
  return QStringList() << "vtk" << "data" << "mesh" << "4D" << "reader" << "writer";
}

QStringList vtkDataMeshPlugin::types() const
{
  return QStringList() << "vtkDataMesh"
                       << "vtkDataMeshWriter"
                       << "vtkDataMesh4DWriter"
                       << "vtkDataMeshWriter"
                       << "vtkDataMesh4DWriter";
}

Q_EXPORT_PLUGIN2(vtkDataMeshPlugin, vtkDataMeshPlugin)
