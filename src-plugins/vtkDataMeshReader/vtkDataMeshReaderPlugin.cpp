/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkDataMeshReaderPlugin.h"
#include <vtkDataMeshReader.h>
#include <vtkDataMesh4DReader.h>

#include <dtkLog/dtkLog.h>

#include <dtkVtkLogForwarder/vtkLogForwarder.h>

// /////////////////////////////////////////////////////////////////
// vtkDataMeshReaderPluginPrivate
// /////////////////////////////////////////////////////////////////

class vtkDataMeshReaderPluginPrivate
{
public:
  vtkDataMeshReaderPluginPrivate()
  {
      forwarder = vtkLogForwarder::New();
  }
  ~vtkDataMeshReaderPluginPrivate()
  {
      forwarder->Delete();
  }
  // Class variables go here.
  vtkLogForwarder* forwarder;
};

// /////////////////////////////////////////////////////////////////
// vtkDataMeshReaderPlugin
// /////////////////////////////////////////////////////////////////

vtkDataMeshReaderPlugin::vtkDataMeshReaderPlugin(QObject *parent) : dtkPlugin(parent), d(new vtkDataMeshReaderPluginPrivate)
{

}

vtkDataMeshReaderPlugin::~vtkDataMeshReaderPlugin()
{
  delete d;

  d = NULL;
}

bool vtkDataMeshReaderPlugin::initialize()
{
    if (!vtkDataMeshReader::registered())   { dtkWarn() << "Unable to register vtkDataMeshReader type"; }
    if (!vtkDataMesh4DReader::registered()) { dtkWarn() << "Unable to register vtkDataMeshReader type"; }
    return true;
}

bool vtkDataMeshReaderPlugin::uninitialize()
{
  return true;
}

QString vtkDataMeshReaderPlugin::name() const
{
  return "vtkDataMeshReaderPlugin";
}

QString vtkDataMeshReaderPlugin::description() const
{
  return "Readers for any VTK point data type.";
}

QString vtkDataMeshReaderPlugin::version() const
{
  return VTKDATAMESHREADERPLUGIN_VERSION;
}

QStringList vtkDataMeshReaderPlugin::tags() const
{
  return QStringList() << "vtk" << "data" << "mesh" << "reader";
}

QStringList vtkDataMeshReaderPlugin::types() const
{
  return QStringList() << "vtkDataMeshReader";
}

Q_EXPORT_PLUGIN2(vtkDataMeshReaderPlugin, vtkDataMeshReaderPlugin)
