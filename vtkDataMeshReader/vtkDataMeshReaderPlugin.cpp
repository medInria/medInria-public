#include "vtkDataMeshReaderPlugin.h"
#include "vtkDataMeshReader.h"
#include "vtkDataMesh4DReader.h"

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

vtkDataMeshReaderPlugin::~vtkDataMeshReaderPlugin(void)
{
  delete d;

  d = NULL;
}

bool vtkDataMeshReaderPlugin::initialize(void)
{
    if (!vtkDataMeshReader::registered())   { dtkWarn() << "Unable to register vtkDataMeshReader type"; }
    if (!vtkDataMesh4DReader::registered()) { dtkWarn() << "Unable to register vtkDataMeshReader type"; }
    return true;
}

bool vtkDataMeshReaderPlugin::uninitialize(void)
{
  return true;
}

QString vtkDataMeshReaderPlugin::name(void) const
{
  return "vtkDataMeshReaderPlugin";
}

QString vtkDataMeshReaderPlugin::description(void) const
{
  return "Readers for any VTK point data type.";
}

QString vtkDataMeshReaderPlugin::version(void) const
{
  return VTKDATAMESHREADERPLUGIN_VERSION;
}

QStringList vtkDataMeshReaderPlugin::tags(void) const
{
  return QStringList() << "vtk" << "data" << "mesh" << "reader";
}

QStringList vtkDataMeshReaderPlugin::types(void) const
{
  return QStringList() << "vtkDataMeshReader";
}

Q_EXPORT_PLUGIN2(vtkDataMeshReaderPlugin, vtkDataMeshReaderPlugin)
